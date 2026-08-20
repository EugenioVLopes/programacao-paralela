import csv
import matplotlib.pyplot as plt

with open("results.csv", newline="") as f:
    rows = list(csv.DictReader(f))

labels = [r["otimizacao"] for r in rows]
series = {
    "Inicialização": [float(r["inicializacao"]) for r in rows],
    "Soma dependente": [float(r["dependente"]) for r in rows],
    "Múltiplos acumuladores": [float(r["multiplos"]) for r in rows],
}

x = range(len(labels))
width = 0.25
fig, ax = plt.subplots(figsize=(9, 5))
for pos, (nome, valores) in enumerate(series.items()):
    ax.bar([i + (pos - 1) * width for i in x], valores, width, label=nome)
ax.set_xticks(list(x), labels)
ax.set_ylabel("Tempo (s)")
ax.set_xlabel("Nível de otimização")
ax.set_title("ILP: dependência entre iterações e tempo de execução")
ax.legend()
ax.grid(axis="y", alpha=0.3)
fig.tight_layout()
fig.savefig("ilp_results.png", dpi=300)
