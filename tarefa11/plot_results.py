#!/usr/bin/env python3
"""Gera gráficos de tempo e speedup no padrão visual das demais tarefas."""
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

plt.figure(figsize=(11, 6))
for (version, label), style in zip(labels.items(), styles):
    threads = sorted(thread for name, thread in groups if name == version)
    medians = [statistics.median(groups[version, thread]) for thread in threads]
    plt.plot(threads, medians, style, markersize=3, label=label)
plt.axhline(sequential, color="gray", linestyle="--", label="sequencial")
if max_threads >= 14:
    plt.axvline(14, color="gray", linestyle=":", label="14 núcleos físicos")
plt.title("Tarefa 11: tempo de execução vs threads")
plt.xlabel("Número de threads")
plt.ylabel("Tempo (s)")
plt.xticks(range(1, max_threads + 1, 2))
plt.grid(True, linestyle="--", alpha=0.6)
plt.legend(fontsize=8)
plt.savefig(path.with_name("tempo_execucao_plot.png"), dpi=300, bbox_inches="tight")
plt.yscale("log")
plt.ylabel("Tempo (s, escala log)")
plt.savefig(path.with_name("tempo_execucao_log_plot.png"), dpi=300, bbox_inches="tight")
plt.close()

plt.figure(figsize=(11, 6))
for (version, label), style in zip(labels.items(), styles):
    threads = sorted(thread for name, thread in groups if name == version)
    speedups = [sequential / statistics.median(groups[version, thread]) for thread in threads]
    plt.plot(threads, speedups, style, markersize=3, label=label)
plt.plot([1, max_threads], [1, max_threads], color="gray", linestyle="--", label="ideal")
if max_threads >= 14:
    plt.axvline(14, color="gray", linestyle=":", label="14 núcleos físicos")
plt.title("Tarefa 11: speedup vs threads")
plt.xlabel("Número de threads")
plt.ylabel("Speedup")
plt.xticks(range(1, max_threads + 1, 2))
plt.grid(True, linestyle="--", alpha=0.6)
plt.legend(fontsize=8)
plt.savefig(path.with_name("speedup_plot.png"), dpi=300, bbox_inches="tight")
plt.close()

print("Gráficos gerados ao lado do CSV.")
