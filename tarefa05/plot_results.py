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

    t1_seq = df.loc[df['threads'] == 1, 'seq_time'].values[0]

    df['speedup_par'] = t1_seq / df['par_time']

    plt.figure(figsize=(10, 6))

    plt.plot(df['threads'], df['threads'], 'k--', label='Speedup ideal')
    plt.plot(df['threads'], df['speedup_par'], 'r-o', label='Paralelo (sem reduction)')

    plt.title('Speedup vs Número de Threads (Contagem de Primos)', fontsize=14)
    plt.xlabel('Número de Threads', fontsize=12)
    plt.ylabel('Speedup $S(p)$', fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend(fontsize=12)

    plt.xticks(df['threads'])

    import multiprocessing
    physical_cores = multiprocessing.cpu_count() // 2
    if physical_cores > 0 and physical_cores < max(df['threads']):
        plt.axvline(x=physical_cores, color='grey', linestyle=':', label='Limite de núcleos físicos')
        plt.legend(fontsize=12)

    plot_path = os.path.join(base_dir, 'speedup_plot.png')
    plt.savefig(plot_path, dpi=300, bbox_inches='tight')
    print(f"Gráfico salvo em {plot_path}")

if __name__ == '__main__':
    main()
