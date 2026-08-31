#!/usr/bin/env python3
"""Gera graficos para os resultados da tarefa 06."""

from __future__ import annotations

import argparse
import csv
import re
from pathlib import Path

import matplotlib.pyplot as plt


CORES = {
    "azul": "#0072B2",
    "laranja": "#E69F00",
    "vermelho": "#D55E00",
    "verde": "#009E73",
    "preto": "#222222",
    "cinza": "#6B7280",
}


def ler_resultados(caminho: Path) -> list[dict[str, float]]:
    with caminho.open(newline="", encoding="utf-8") as arquivo:
        linhas = list(csv.DictReader(arquivo))

    if not linhas:
        raise ValueError(f"{caminho} nao contem resultados.")

    campos_numericos = [campo for campo in linhas[0] if campo != "threads"]
    resultados = []
    for linha in linhas:
        resultado = {"threads": int(linha["threads"])}
        resultado.update({campo: float(linha[campo]) for campo in campos_numericos})
        resultados.append(resultado)
    return resultados


def ler_nucleos_fisicos(caminho: Path, threads_maximas: int) -> int:
    if not caminho.exists():
        return threads_maximas

    for linha in caminho.read_text(encoding="utf-8").splitlines():
        resultado = re.match(r"Nucleos fisicos detectados: (\d+)", linha)
        if resultado:
            return int(resultado.group(1))
    return threads_maximas


def estilo_eixos(eixo: plt.Axes) -> None:
    eixo.spines["top"].set_visible(False)
    eixo.spines["right"].set_visible(False)
    eixo.grid(axis="y", color="#D1D5DB", linewidth=0.7)
    eixo.set_axisbelow(True)


def configurar_eixo_threads(eixo: plt.Axes, threads: list[int]) -> None:
    if len(threads) <= 12:
        eixo.set_xticks(threads)
        return

    rotulos = [thread for thread in threads if thread == 1 or thread % 2 == 0]
    if threads[-1] not in rotulos:
        rotulos.append(threads[-1])
    eixo.set_xticks(rotulos)
    eixo.set_xticks(threads, minor=True)
    eixo.tick_params(axis="x", which="minor", length=3)


def salvar(figura: plt.Figure, destino: Path) -> None:
    figura.savefig(destino.with_suffix(".png"), dpi=300, bbox_inches="tight")
    figura.savefig(destino.with_suffix(".pdf"), bbox_inches="tight")
    plt.close(figura)


def grafico_race(resultados: list[dict[str, float]], destino: Path) -> None:
    threads = [resultado["threads"] for resultado in resultados]
    medias = [resultado["e1_pi_mean"] for resultado in resultados]
    minimos = [resultado["e1_pi_min"] for resultado in resultados]
    maximos = [resultado["e1_pi_max"] for resultado in resultados]
    referencia = resultados[0]["e2_pi"]

    figura, eixo = plt.subplots(figsize=(7.2, 4.2), constrained_layout=True)
    eixo.fill_between(
        threads,
        minimos,
        maximos,
        color=CORES["vermelho"],
        alpha=0.18,
        label="intervalo observado em e1",
    )
    eixo.plot(
        threads,
        medias,
        color=CORES["vermelho"],
        marker="o",
        linewidth=2,
        label="média de e1",
    )
    eixo.axhline(
        referencia,
        color=CORES["preto"],
        linestyle="--",
        linewidth=1.5,
        label=r"valor sincronizado, $\hat{\pi}=3{,}1415764$",
    )
    eixo.set_xlabel("Threads OpenMP")
    eixo.set_ylabel(r"Estimativa de $\pi$")
    configurar_eixo_threads(eixo, threads)
    eixo.set_ylim(bottom=0)
    eixo.legend(frameon=False, loc="upper right", bbox_to_anchor=(1.0, 0.85), fontsize=9)
    estilo_eixos(eixo)
    salvar(figura, destino)


def grafico_tempos(resultados: list[dict[str, float]], destino: Path) -> None:
    threads = [resultado["threads"] for resultado in resultados]
    tempo_e2 = [resultado["e2_time_median_s"] for resultado in resultados]
    desvio_e2 = [resultado["e2_time_std_s"] for resultado in resultados]
    tempo_e3 = [resultado["e3_time_median_s"] for resultado in resultados]
    desvio_e3 = [resultado["e3_time_std_s"] for resultado in resultados]

    figura, eixo = plt.subplots(figsize=(7.2, 4.2), constrained_layout=True)
    eixo.errorbar(
        threads,
        tempo_e2,
        yerr=desvio_e2,
        color=CORES["vermelho"],
        marker="o",
        capsize=3,
        linewidth=2,
        label="e2, critical por ponto",
    )
    eixo.errorbar(
        threads,
        tempo_e3,
        yerr=desvio_e3,
        color=CORES["azul"],
        marker="s",
        capsize=3,
        linewidth=2,
        label="e3, contador local",
    )
    eixo.set_yscale("log")
    eixo.set_xlabel("Threads OpenMP")
    eixo.set_ylabel("Tempo de execução (s, escala logarítmica)")
    configurar_eixo_threads(eixo, threads)
    eixo.legend(frameon=False, loc="upper left")
    estilo_eixos(eixo)
    salvar(figura, destino)


def grafico_speedup(
    resultados: list[dict[str, float]], nucleos_fisicos: int, destino: Path
) -> None:
    threads = [resultado["threads"] for resultado in resultados]
    base_e2 = resultados[0]["e2_time_median_s"]
    base_e3 = resultados[0]["e3_time_median_s"]
    speedup_e2 = [base_e2 / resultado["e2_time_median_s"] for resultado in resultados]
    speedup_e3 = [base_e3 / resultado["e3_time_median_s"] for resultado in resultados]

    figura, eixo = plt.subplots(figsize=(7.2, 4.2), constrained_layout=True)
    eixo.plot(
        threads,
        threads,
        color=CORES["cinza"],
        linestyle="--",
        linewidth=1.5,
        label="speedup ideal",
    )
    eixo.plot(
        threads,
        speedup_e2,
        color=CORES["vermelho"],
        marker="o",
        linewidth=2,
        label="e2, critical por ponto",
    )
    eixo.plot(
        threads,
        speedup_e3,
        color=CORES["azul"],
        marker="s",
        linewidth=2,
        label="e3, contador local",
    )
    if nucleos_fisicos in threads:
        eixo.axvline(
            nucleos_fisicos,
            color=CORES["verde"],
            linestyle=":",
            linewidth=1.5,
            label=f"{nucleos_fisicos} núcleos físicos",
        )
    eixo.set_xlabel("Threads OpenMP")
    eixo.set_ylabel("Speedup em relação a uma thread")
    configurar_eixo_threads(eixo, threads)
    eixo.set_ylim(bottom=0)
    eixo.legend(frameon=False, loc="upper left")
    estilo_eixos(eixo)
    salvar(figura, destino)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("results", nargs="?", default="results.csv")
    parser.add_argument("environment", nargs="?", default="environment.txt")
    args = parser.parse_args()

    resultados = ler_resultados(Path(args.results))
    nucleos_fisicos = ler_nucleos_fisicos(
        Path(args.environment), resultados[-1]["threads"]
    )

    grafico_race(resultados, Path("race_condition_plot"))
    grafico_tempos(resultados, Path("tempo_execucao_plot"))
    grafico_speedup(resultados, nucleos_fisicos, Path("speedup_plot"))


if __name__ == "__main__":
    main()
