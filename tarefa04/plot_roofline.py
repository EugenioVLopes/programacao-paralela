#!/usr/bin/env python3
"""
Roofline Model, Tarefa 04, DCA3703.

Gera um gráfico log-log com os tetos de desempenho do processador
(pico de FLOP/s) e da largura de banda de memória, plotando os
pontos operacionais dos kernels memory-bound e cpu-bound medidos.

Hardware de referência: Intel Core i7-14700HX
  - 14 núcleos (8P + 6E), DDR5-5600 dual-channel
  - Pico teórico FP64 (8P-cores AVX2 FMA):
      8 cores × 2 FMA × 4 doubles/ciclo × 2 (FMA) × ~3.5 GHz ≈ 448 GFLOP/s
    (valor conservador com boost; usamos estimativa empírica ajustada)
  - Largura de banda pico DDR5-5600 dual-channel: ~89.6 GB/s
    (prática medida com STREAM ~50-65 GB/s)
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import pandas as pd
import os

# ──────────────────────────────────────────────────────────────
# Parâmetros de hardware (Intel i7-14700HX)
# ──────────────────────────────────────────────────────────────
PEAK_GFLOPS = 350.0       # GFLOP/s teórico FP64 (estimativa conservadora)
PEAK_BW_GBS = 55.0        # GB/s prática (STREAM-like, não teórica)

# Sub-tetos para núcleo único e sem vetorização
PEAK_GFLOPS_1CORE = 28.0  # 1 P-core, ~3.5 GHz, escalar FP64
PEAK_BW_1CORE_GBS = 20.0  # BW efetiva de 1 core (limitada por latência)

N = 5_000_000             # Número de elementos usado nos experimentos

# ──────────────────────────────────────────────────────────────
# Cálculo da intensidade aritmética dos kernels
# ──────────────────────────────────────────────────────────────

# Kernel Memory-bound: C[i] = A[i] + B[i]
#   FLOPs: 1 ADD por elemento
#   Bytes: 2 leituras + 1 escrita = 3 × 8 = 24 bytes/elemento
AI_MEM = 1.0 / 24.0       # ≈ 0.042 FLOP/byte

# Kernel CPU-bound: 50 iterações de val = sqrt(val + j*0.01) + sin(val)
#   Por iteração interna:
#     j*0.01       → 1 MUL
#     val + ...    → 1 ADD
#     sqrt(...)    → ~8 FLOPs (implementação em software via libm)
#     sin(val)     → ~10 FLOPs (redução de argumento + polinômio minimax)
#     + (soma)     → 1 ADD
#   Total por iteração interna: ~21 FLOPs
#   50 iterações: ~1050 FLOPs por elemento
#   Bytes: 1 leitura + 1 escrita = 2 × 8 = 16 bytes/elemento
FLOPS_CPU_PER_ELEM = 50 * 21
AI_CPU = FLOPS_CPU_PER_ELEM / 16.0  # ≈ 65.6 FLOP/byte


def compute_gflops(time_s, flops_per_elem, n):
    """Retorna GFLOP/s a partir do tempo medido."""
    return (flops_per_elem * n) / (time_s * 1e9)


def main():
    base_dir = os.path.dirname(os.path.abspath(__file__))
    csv_path = os.path.join(base_dir, 'results.csv')

    df = pd.read_csv(csv_path)

    # ── Pontos operacionais para cada contagem de threads ──
    # Pontos que mostram o início, a escala e a saturação.
    threads_sel = [1, 4, 8, 14, 26, 28]

    mem_points = []  # (AI, GFLOP/s, label)
    cpu_points = []

    for t in threads_sel:
        row = df[df['threads'] == t]
        if row.empty:
            continue
        t_mem = row['memory_bound'].values[0]
        t_cpu = row['cpu_bound'].values[0]

        gf_mem = compute_gflops(t_mem, 1, N)         # 1 FLOP/elem
        gf_cpu = compute_gflops(t_cpu, FLOPS_CPU_PER_ELEM, N)

        mem_points.append((AI_MEM, gf_mem, f'{t}T'))
        cpu_points.append((AI_CPU, gf_cpu, f'{t}T'))

    # ── Construção do gráfico Roofline ──
    fig, ax = plt.subplots(figsize=(10, 6.5))

    # Faixa de intensidade aritmética
    ai_range = np.logspace(-2, 3, 500)

    # Teto multi-core
    roofline_multi = np.minimum(PEAK_GFLOPS, PEAK_BW_GBS * ai_range)
    ax.plot(ai_range, roofline_multi, 'k-', linewidth=2.2,
            label=f'Teto multinúcleo ({PEAK_GFLOPS:.0f} GFLOP/s, {PEAK_BW_GBS:.0f} GB/s)')

    # Teto single-core (referência)
    roofline_single = np.minimum(PEAK_GFLOPS_1CORE, PEAK_BW_1CORE_GBS * ai_range)
    ax.plot(ai_range, roofline_single, 'k--', linewidth=1.2, alpha=0.5,
            label=f'Teto 1 núcleo ({PEAK_GFLOPS_1CORE:.0f} GFLOP/s, {PEAK_BW_1CORE_GBS:.0f} GB/s)')

    # Preencher a região sob o teto multi-core
    ax.fill_between(ai_range, roofline_multi, alpha=0.04, color='grey')

    # Ponto de crista (interseção BW × AI = pico GFLOP/s)
    ridge_ai = PEAK_GFLOPS / PEAK_BW_GBS
    ax.axvline(x=ridge_ai, color='grey', linestyle=':', linewidth=0.8, alpha=0.6)
    ax.annotate(f'Ponto de Crista\nAI = {ridge_ai:.1f}',
                xy=(ridge_ai, PEAK_GFLOPS * 0.6),
                fontsize=8, color='grey', ha='center',
                bbox=dict(boxstyle='round,pad=0.3', fc='white', ec='grey', alpha=0.8))

    # ── Plotar pontos operacionais ──
    # Memory-bound (azuis)
    for ai, gf, lbl in mem_points:
        ax.plot(ai, gf, 'o', color='#2563EB', markersize=9, zorder=5,
                markeredgecolor='white', markeredgewidth=0.8)
        ax.annotate(lbl, (ai, gf), textcoords='offset points',
                    xytext=(8, -4), fontsize=7.5, color='#2563EB', fontweight='bold')

    # CPU-bound (vermelhos)
    for ai, gf, lbl in cpu_points:
        ax.plot(ai, gf, 's', color='#DC2626', markersize=9, zorder=5,
                markeredgecolor='white', markeredgewidth=0.8)
        ax.annotate(lbl, (ai, gf), textcoords='offset points',
                    xytext=(8, -4), fontsize=7.5, color='#DC2626', fontweight='bold')

    # Marcadores de legenda (invisíveis, só para a legenda)
    ax.plot([], [], 'o', color='#2563EB', markersize=8, label='Limitado por memória (C=A+B)')
    ax.plot([], [], 's', color='#DC2626', markersize=8, label='Limitado por CPU (sqrt+sin ×50)')

    # Regiões anotadas
    ax.text(0.02, 0.5, 'LIMITADO\nPOR MEMÓRIA', transform=ax.transAxes,
            fontsize=10, color='#2563EB', alpha=0.25, fontweight='bold',
            ha='left', va='center', rotation=48)
    ax.text(0.75, 0.85, 'LIMITADO\nPOR CPU', transform=ax.transAxes,
            fontsize=10, color='#DC2626', alpha=0.25, fontweight='bold',
            ha='center', va='center')

    # ── Formatação ──
    ax.set_xscale('log')
    ax.set_yscale('log')
    ax.set_xlabel('Intensidade Aritmética (FLOP/byte)', fontsize=12)
    ax.set_ylabel('Desempenho (GFLOP/s)', fontsize=12)
    ax.set_title('Modelo Roofline — Tarefa 04 (i7-14700HX)', fontsize=14, fontweight='bold')

    ax.set_xlim(0.01, 500)
    ax.set_ylim(0.01, 800)

    ax.xaxis.set_major_formatter(ticker.ScalarFormatter())
    ax.yaxis.set_major_formatter(ticker.ScalarFormatter())
    ax.tick_params(axis='both', which='major', labelsize=10)

    ax.grid(True, which='both', linestyle='--', alpha=0.3)
    ax.legend(loc='lower right', fontsize=9, framealpha=0.9)

    plt.tight_layout()
    out_path = os.path.join(base_dir, 'roofline_plot.png')
    fig.savefig(out_path, dpi=300, bbox_inches='tight')
    print(f'Roofline salvo em {out_path}')
    plt.close()


if __name__ == '__main__':
    main()
