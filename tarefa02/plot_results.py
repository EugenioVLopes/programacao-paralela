#!/usr/bin/env python3
import json
import matplotlib.pyplot as plt

with open('results.json') as f:
    data = json.load(f)

Ns = [d['N'] for d in data]
row_tempos = [d['row_time'] for d in data]
col_tempos = [d['col_time'] for d in data]
fig, ax1 = plt.subplots(figsize=(8, 5))

# Plot 1: Time comparison
ax1.plot(Ns, row_tempos, 'bo-', linewidth=2, markersize=7, label='Row-major (laços por linha)')
ax1.plot(Ns, col_tempos, 'rs-', linewidth=2, markersize=7, label='Column-major (laços por coluna)')
ax1.set_xlabel('Tamanho da Matriz (N)', fontsize=12)
ax1.set_ylabel('Tempo de Execução (s)', fontsize=12)
ax1.set_title('Tempo de Execução vs Tamanho da Matriz', fontsize=14)
ax1.grid(True, ls="-", alpha=0.3)
ax1.legend(fontsize=11)
ax1.tick_params(axis='both', which='major', labelsize=10)

fig.suptitle('Acesso à memória em MxV: row-major vs column-major', fontsize=16)

plt.tight_layout()
plt.savefig('mxv_memory_analysis.png', dpi=300, bbox_inches='tight')
print("Graph saved as mxv_memory_analysis.png")
