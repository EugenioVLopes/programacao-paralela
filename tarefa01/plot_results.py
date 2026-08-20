#!/usr/bin/env python3
import json
import matplotlib.pyplot as plt

with open('results.json') as f:
    data = json.load(f)

iters = [d['iteracoes'] for d in data]
tempos = [d['tempo'] for d in data]
erros = [d['erro'] for d in data]

# Create figure with two subplots
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

# Plot 1: Iterations vs Time
ax1.plot(iters, tempos, 'bo-', linewidth=2, markersize=8)
ax1.set_xlabel('Número de Iterações', fontsize=12)
ax1.set_ylabel('Tempo de Execução (s)', fontsize=12)
ax1.set_title('Tempo de Execução vs Número de Iterações', fontsize=14)
ax1.set_xscale('log')
ax1.set_yscale('log')
ax1.grid(True, which="both", ls="-", alpha=0.3)
ax1.tick_params(axis='both', which='major', labelsize=10)

# Plot 2: Iterations vs Error
ax2.plot(iters, erros, 'rs-', linewidth=2, markersize=8)
ax2.set_xlabel('Número de Iterações', fontsize=12)
ax2.set_ylabel('Erro Absoluto', fontsize=12)
ax2.set_title('Erro Absoluto vs Número de Iterações', fontsize=14)
ax2.set_xscale('log')
ax2.set_yscale('log')
ax2.grid(True, which="both", ls="-", alpha=0.3)
ax2.tick_params(axis='both', which='major', labelsize=10)

# Add main title
fig.suptitle('Análise de Aproximação de π com Série de Leibniz', fontsize=16, y=1.02)

# Adjust layout
plt.tight_layout()

# Save the figure
plt.savefig('pi_approximation_analysis.png', dpi=300, bbox_inches='tight')
print("Graph saved as pi_approximation_analysis.png")