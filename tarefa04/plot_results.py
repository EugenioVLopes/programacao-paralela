import pandas as pd
import matplotlib.pyplot as plt
import os

def main():
    # Caminho do diretório atual (tarefa04)
    base_dir = os.path.dirname(os.path.abspath(__file__))
    csv_path = os.path.join(base_dir, 'results.csv')
    
    if not os.path.exists(csv_path):
        print(f"Erro: Arquivo {csv_path} não encontrado.")
        return

    df = pd.read_csv(csv_path)

    # Pegando os tempos com 1 thread (base para speedup)
    t1_mem = df.loc[df['threads'] == 1, 'memory_bound'].values[0]
    t1_cpu = df.loc[df['threads'] == 1, 'cpu_bound'].values[0]

    # Calculando Speedup
    df['speedup_mem'] = t1_mem / df['memory_bound']
    df['speedup_cpu'] = t1_cpu / df['cpu_bound']

    plt.figure(figsize=(10, 6))
    
    # Plota Speedup Ideal
    plt.plot(df['threads'], df['threads'], 'k--', label='Speedup Ideal')
    
    # Plota Memory-bound
    plt.plot(df['threads'], df['speedup_mem'], 'b-o', label='Memory-bound (Soma de vetores)')
    
    # Plota CPU-bound
    plt.plot(df['threads'], df['speedup_cpu'], 'r-s', label='CPU-bound (Operações intensas)')

    plt.title('Speedup vs Número de Threads', fontsize=14)
    plt.xlabel('Número de Threads', fontsize=12)
    plt.ylabel('Speedup $S(p)$', fontsize=12)
    plt.grid(True, linestyle='--', alpha=0.7)
    plt.legend(fontsize=12)
    
    plt.xticks(df['threads'])
    
    # Adicionando linha indicando provável transição para threads lógicas (SMT)
    # nproc/2 costuma ser o número de cores físicos
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
