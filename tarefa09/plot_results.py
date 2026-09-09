#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import os
def main():
    base = os.path.dirname(os.path.abspath(__file__))
    df = pd.read_csv(os.path.join(base, 'results.csv'))
    plt.figure(figsize=(11, 6))
    plt.plot(df['threads'], df['v1_critical_nomeado_s'], 'r-o', markersize=3, label='v1 critical nomeado (2 listas)')
    plt.plot(df['threads'], df['v2_locks_s'], 'b-s', markersize=3, label='v2 locks explicitos (N listas)')
    plt.title('Tempo de execucao vs Threads (tarefa09)', fontsize=13)
    plt.xlabel('Numero de threads')
    plt.ylabel('Tempo (s)')
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.xticks(list(range(1, int(df['threads'].max())+1, 2)))
    plt.axvline(x=14, color='grey', linestyle=':', label='14 nucleos fisicos')
    plt.legend(fontsize=9)
    p1 = os.path.join(base, 'tempo_execucao_plot.png')
    plt.savefig(p1, dpi=300, bbox_inches='tight')
    print('salvo ' + p1)
    plt.figure(figsize=(8, 5))
    row = df[df['threads'] == min(8, int(df['threads'].max()))].iloc[0]
    vals = [row['v1_critical_nomeado_s'], row['v2_locks_s']]
    labels = ['v1\ncrit nomeado', 'v2\nlocks']
    colors = ['#D55E00', '#0072B2']
    plt.bar(labels, vals, color=colors)
    plt.title('Comparacao v1 x v2', fontsize=13)
    plt.ylabel('Tempo (s)')
    for i, v in enumerate(vals):
        plt.text(i, v * 1.03, f'{v:.4f}s', ha='center', fontsize=9)
    plt.grid(axis='y', linestyle='--', alpha=0.6)
    p2 = os.path.join(base, 'comparacao_versoes.png')
    plt.savefig(p2, dpi=300, bbox_inches='tight')
    print('salvo ' + p2)
if __name__ == '__main__':
    main()
