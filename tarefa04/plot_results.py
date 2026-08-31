import pandas as pd
import matplotlib.pyplot as plt
import os

def main():
    base_dir = os.path.dirname(os.path.abspath(__file__))
    csv_path = os.path.join(base_dir, 'results.csv')

    if not os.path.exists(csv_path):
        print(f"Erro: Arquivo {csv_path} não encontrado.")
        return

    df = pd.read_csv(csv_path)

    fig, ax = plt.subplots(figsize=(10, 6))

    ax.errorbar(df['threads'], df['cpu_bound_mean'], yerr=df['cpu_bound_std'],
                fmt='r-s', markersize=5, capsize=3, capthick=1,
                label='Limitado por CPU (sqrt+sin ×50)')
    ax.errorbar(df['threads'], df['memory_bound_mean'], yerr=df['memory_bound_std'],
                fmt='b-o', markersize=5, capsize=3, capthick=1,
                label='Limitado por memória (C=A+B)')

    ax.set_yscale('log')
    ax.set_xlabel('Número de threads', fontsize=12)
    ax.set_ylabel('Tempo de execução (s)', fontsize=12)
    ax.set_title('Tempo de execução vs. número de threads (média de 10 execuções)', fontsize=14,
                 fontweight='bold')

    ax.grid(True, which='both', linestyle='--', alpha=0.3)
    ax.legend(fontsize=11)

    ax.set_xticks(df['threads'])
    ax.set_xticklabels([str(t) if t in [1,2,4,8,14,20,28] else ''
                        for t in df['threads']], fontsize=9)

    plt.tight_layout()
    plot_path = os.path.join(base_dir, 'speedup_plot.png')
    fig.savefig(plot_path, dpi=300, bbox_inches='tight')
    print(f"Gráfico salvo em {plot_path}")
    plt.close()

if __name__ == '__main__':
    main()
