#!/usr/bin/env python3
"""Gera os gráficos de tempo e de comparação das estratégias."""
import csv
from collections import defaultdict
from pathlib import Path
import statistics
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

path = Path(sys.argv[1])
groups = defaultdict(list)
with path.open() as file:
    for row in csv.DictReader(file):
        groups[row["version"], int(row["threads"])].append(float(row["seconds"]))

labels = {
    "v1_static": "v1 static",
    "v2_static_collapse": "v2 static + collapse",
    "v3_dynamic": "v3 dynamic",
    "v4_dynamic_collapse": "v4 dynamic + collapse",
    "v5_guided_collapse": "v5 guided + collapse",
    "v6_guided": "v6 guided",
}
styles = ["r-o", "b-s", "g-^", "m-D", "c-v", "k-x"]
sequential = statistics.median(groups["v0_seq", 1])
max_threads = max(thread for version, thread in groups if version != "v0_seq")
physical_threads = min(14, max_threads)

plt.figure(figsize=(11, 6))
for (version, label), style in zip(labels.items(), styles):
    threads = sorted(thread for name, thread in groups if name == version and thread <= physical_threads)
    medians = [statistics.median(groups[version, thread]) for thread in threads]
    plt.plot(threads, medians, style, markersize=3, label=label)
plt.axhline(sequential, color="gray", linestyle="--", label="sequencial")
if max_threads >= 14:
    plt.axvline(14, color="gray", linestyle=":", label="14 núcleos físicos")
plt.title("Tarefa 11: tempo de execução vs threads")
plt.xlabel("Número de threads")
plt.ylabel("Tempo (s)")
plt.xticks(range(1, physical_threads + 1, 2))
plt.grid(True, linestyle="--", alpha=0.6)
plt.legend(fontsize=8)
plt.yscale("log")
plt.ylabel("Tempo (s, escala log)")
plt.savefig(path.with_name("tempo_execucao_log_plot.png"), dpi=300, bbox_inches="tight")
plt.close()

comparison_threads = physical_threads
comparison = [statistics.median(groups[version, comparison_threads]) for version in labels]
plt.figure(figsize=(10, 5.5))
bars = plt.bar(list(labels.values()), comparison, color=["#d62728", "#1f77b4", "#2ca02c", "#9467bd", "#17becf", "#222222"])
plt.ylabel("Tempo mediano (s)")
plt.title(f"Tarefa 11: comparação das estratégias com {comparison_threads} threads")
plt.xticks(rotation=18, ha="right")
plt.grid(axis="y", linestyle="--", alpha=0.6)
for bar, value in zip(bars, comparison):
    plt.text(bar.get_x() + bar.get_width() / 2, bar.get_height(), f"{value:.4f}", ha="center", va="bottom", fontsize=8)
plt.tight_layout()
plt.savefig(path.with_name("estrategias_14_threads.png"), dpi=300, bbox_inches="tight")
plt.close()

print("Gráficos gerados ao lado do CSV.")
