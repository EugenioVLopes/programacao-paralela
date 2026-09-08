#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import os
def main():
    base = os.path.dirname(os.path.abspath(__file__))
    df = pd.read_csv(os.path.join(base, 'results.csv'))
    plt.figure(figsize=(11, 6))
    plt.plot(df['threads'], df['v1_critical_rand_s'], 'r-o', markersize=3, label='v1 critical+rand')
    plt.plot(df['threads'], df['v2_vector_rand_s'], 'b-s', markersize=3, label='v2 vetor+rand')
    plt.plot(df['threads'], df['v3_critical_randr_s'], 'g-^', markersize=4, label='v3 critical+rand_r')
    plt.plot(df['threads'], df['v4_vector_randr_s'], 'm-D', markersize=3, label='v4 vetor+rand_r')
    plt.plot(df['threads'], df['v4pad_vector_randr_pad_s'], 'k-x', markersize=4, label='v4-pad vetor+rand_r+pad64')
    plt.title('Tempo de execucao vs Threads 1-28 (N=5M)', fontsize=13)
    plt.xlabel('Numero de threads')
    plt.ylabel('Tempo (s)')
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.legend(fontsize=9)
    plt.xticks(list(range(1, int(df['threads'].max())+1, 2)))
    plt.axvline(x=14, color='grey', linestyle=':', label='14 nucleos fisicos')
    plt.legend(fontsize=8)
    p1 = os.path.join(base, 'tempo_execucao_plot.png')
    plt.savefig(p1, dpi=300, bbox_inches='tight')
    print('salvo ' + p1)
    plt.yscale('log')
    plt.title('Tempo de execucao vs Threads 1-28 (N=5M, escala log)', fontsize=13)
    plt.ylabel('Tempo (s, log)')
    plt.grid(True, which='both', linestyle='--', alpha=0.6)
    p1log = os.path.join(base, 'tempo_execucao_log_plot.png')
    plt.savefig(p1log, dpi=300, bbox_inches='tight')
    print('salvo ' + p1log)
    plt.figure(figsize=(11, 6))
    row8 = df[df['threads'] == 8].iloc[0]
    vals = [row8['v1_critical_rand_s'], row8['v2_vector_rand_s'], row8['v3_critical_randr_s'], row8['v4_vector_randr_s'], row8['v4pad_vector_randr_pad_s']]
    labels = ['v1\ncrit+rand', 'v2\nvet+rand', 'v3\ncrit+rand_r', 'v4\nvet+rand_r', 'v4-pad\nvet+pad']
    colors = ['#D55E00', '#0072B2', '#009E73', '#CC79A7', '#222222']
    plt.bar(labels, vals, color=colors)
    plt.title('Comparacao das 5 versoes com 8 threads (N=5M)', fontsize=13)
    plt.ylabel('Tempo (s)')
    for i, v in enumerate(vals):
        plt.text(i, v * 1.03, f'{v:.4f}s', ha='center', fontsize=9)
    plt.grid(axis='y', linestyle='--', alpha=0.6)
    p2 = os.path.join(base, 'comparacao_versoes.png')
    plt.savefig(p2, dpi=300, bbox_inches='tight')
    print('salvo ' + p2)
if __name__ == '__main__':
    main()
