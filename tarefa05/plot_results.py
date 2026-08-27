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

    plt.figure(figsize=(10, 6))

    plt.plot(df['threads'], df['seq_time'], 'k--', label='Sequencial')
    plt.plot(df['threads'], df['par_time'], 'r-o', label='Paralelo')

    plt.title('Tempo de Execução vs Número de Threads (Contagem de Primos)', fontsize=14)
    plt.xlabel('Número de Threads', fontsize=12)
    plt.ylabel('Tempo (s)', fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend(fontsize=12)

    plt.xticks(df['threads'])

    import multiprocessing
    physical_cores = multiprocessing.cpu_count() // 2
    if physical_cores > 0 and physical_cores < max(df['threads']):
        plt.axvline(x=physical_cores, color='grey', linestyle=':', label='Limite de núcleos físicos')
        plt.legend(fontsize=12)

    tempo_path = os.path.join(base_dir, 'tempo_execucao_plot.png')
    plt.savefig(tempo_path, dpi=300, bbox_inches='tight')
    print(f"Gráfico salvo em {tempo_path}")

    # Gráfico de contagem
    plt.figure(figsize=(10, 6))

    plt.axhline(y=df['seq_count'].iloc[0], color='k', linestyle='--', label='Sequencial (348.513)')
    plt.plot(df['threads'], df['par_count'], 'r-o', label='Paralelo')

    plt.title('Contagem de Primos vs Número de Threads', fontsize=14)
    plt.xlabel('Número de Threads', fontsize=12)
    plt.ylabel('Primos encontrados', fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend(fontsize=12)

    plt.xticks(df['threads'])

    if physical_cores > 0 and physical_cores < max(df['threads']):
        plt.axvline(x=physical_cores, color='grey', linestyle=':')

    contagem_path = os.path.join(base_dir, 'contagem_primos_plot.png')
    plt.savefig(contagem_path, dpi=300, bbox_inches='tight')
    print(f"Gráfico salvo em {contagem_path}")

if __name__ == '__main__':
    main()
