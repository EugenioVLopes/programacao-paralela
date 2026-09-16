#!/usr/bin/env python3
"""Compara chunks equivalentes: linhas sem collapse e células com collapse."""
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
        key = row["schedule"], row["unit"], int(row["equivalent_rows"])
        groups[key].append(float(row["seconds"]))

series = [
    (("dynamic", "linhas"), "dynamic sem collapse", "r-o"),
    (("dynamic", "celulas"), "dynamic com collapse", "b-s"),
    (("guided", "linhas"), "guided sem collapse", "g-^"),
    (("guided", "celulas"), "guided com collapse", "m-D"),
]

plt.figure(figsize=(10, 6))
for (schedule, unit), label, style in series:
    rows = sorted(size for sch, unt, size in groups if sch == schedule and unt == unit)
    medians = [statistics.median(groups[schedule, unit, size]) for size in rows]
    plt.plot(rows, medians, style, markersize=5, label=label)
plt.xscale("log", base=2)
plt.yscale("log")
plt.xticks([1, 4, 16, 64], ["1", "4", "16", "64"])
plt.title("Tarefa 11: efeito do chunksize com 8 threads")
plt.xlabel("Volume equivalente do chunk (linhas da grade)")
plt.ylabel("Tempo (s, escala log)")
plt.grid(True, which="both", linestyle="--", alpha=0.6)
plt.legend(fontsize=9)
plt.savefig(path.with_name("chunksize_plot.png"), dpi=300, bbox_inches="tight")
plt.close()
print("Gráfico de chunks gerado ao lado do CSV.")
