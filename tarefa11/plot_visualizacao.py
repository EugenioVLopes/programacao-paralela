#!/usr/bin/env python3
"""Gera a figura do benchmark ou, com --animacao, os quadros da demonstração."""
import argparse
from pathlib import Path
import subprocess
import tempfile

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

ROOT = Path(__file__).resolve().parent
GRID_SIZE = 512
TIMES = (0, 500)


def initial_field() -> np.ndarray:
    y, x = np.indices((GRID_SIZE, GRID_SIZE))
    center = (GRID_SIZE - 1) / 2.0
    sigma = GRID_SIZE / 10.0
    field = 0.1 * np.exp(-((x - center) ** 2 + (y - center) ** 2) / (2 * sigma**2))
    field[[0, -1], :] = 0.0
    field[:, [0, -1]] = 0.0
    return field


def final_field() -> np.ndarray:
    with tempfile.TemporaryDirectory() as directory:
        binary = Path(directory) / "ns_v0_vis"
        output = Path(directory) / "campo.txt"
        subprocess.run(
            ["gcc", "-std=c11", "-O2", "-Wall", "-Wextra", "v0_seq.c", "-lm", "-o", str(binary)],
            cwd=ROOT,
            check=True,
        )
        subprocess.run(
            [str(binary), "0", str(output)],
            cwd=ROOT,
            check=True,
            stdout=subprocess.DEVNULL,
        )
        data = np.loadtxt(output)
    if data.shape != (GRID_SIZE, GRID_SIZE):
        raise ValueError(f"campo com forma inesperada: {data.shape}")
    return data


def central_moment(field: np.ndarray) -> float:
    y, x = np.indices(field.shape)
    total = field.sum()
    if total == 0:
        return 0.0
    cx = (field * x).sum() / total
    cy = (field * y).sum() / total
    return float((field * ((x - cx) ** 2 + (y - cy) ** 2)).sum() / total)


def plot_diffusion(fields: dict[int, np.ndarray]) -> None:
    vmax = max(float(field.max()) for field in fields.values())
    fig, axes = plt.subplots(2, len(TIMES), figsize=(9, 7), constrained_layout=True)
    center = GRID_SIZE // 2

    for column, time_step in enumerate(TIMES):
        field = fields[time_step]
        axis = axes[0, column]
        image = axis.imshow(field, origin="lower", cmap="viridis", vmin=0, vmax=vmax)
        axis.set_title(f"t = {time_step} passos\npico={field.max():.4f}")
        axis.set_xticks([])
        axis.set_yticks([])

        axis = axes[1, column]
        axis.plot(field[center, :], color="#1f77b4", linewidth=2)
        axis.set_ylim(0, vmax * 1.05)
        axis.grid(alpha=0.3)
        axis.set_xlabel("posição x na linha central")
        if column == 0:
            axis.set_ylabel("u")
        axis.set_title(f"raio² médio={central_moment(field):.1f}")

    colorbar = fig.colorbar(image, ax=axes[0, :], shrink=0.75)
    colorbar.set_label("u(x,y)")
    fig.suptitle("Difusão suave da perturbação gaussiana", fontsize=15)
    fig.savefig(ROOT / "visualizacao_difusao.png", dpi=180)
    plt.close(fig)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--animacao", action="store_true",
                        help="gera 201 quadros em C e uma figura de 0 a 2.100.000 passos")
    if parser.parse_args().animacao:
        generate_animation()
        return
    fields = {0: initial_field(), 500: final_field()}
    plot_diffusion(fields)
    print("Gerada: visualizacao_difusao.png")


def generate_animation() -> None:
    with tempfile.TemporaryDirectory(dir=ROOT) as directory:
        directory = Path(directory)
        binary = directory / "gerar_quadros"
        output = directory / "quadros_difusao.bin"
        subprocess.run(["gcc", "-std=c11", "-O2", "-Wall", "-Wextra", "-Werror",
                        str(ROOT / "gerar_quadros.c"), "-lm", "-o", str(binary)], check=True)
        with output.open("wb") as stream:
            subprocess.run([str(binary)], stdout=stream, check=True)
        data = np.fromfile(output, dtype="<f4")
        grid, samples, steps, count = map(int, data[:4])
        frames = data[5:].reshape(count, 1 + samples**2)
        fig, axes = plt.subplots(1, 2, figsize=(10, 4), constrained_layout=True)
        for axis, index in zip(axes, (0, count - 1)):
            image = axis.imshow(frames[index, 1:].reshape(samples, samples),
                                origin="lower", cmap="viridis", vmin=0, vmax=0.1,
                                extent=(0, grid - 1, 0, grid - 1))
            axis.set_title(f"Passo {steps if index else 0:,}".replace(",", ".") +
                           f"\npico ≈ {frames[index, 0]:.5f}")
            axis.set_xlabel("coluna j")
            axis.set_ylabel("linha i")
        fig.colorbar(image, ax=axes, label="u(x,y)", shrink=0.8)
        fig.suptitle("Demonstração: difusão ao longo de 2.100.000 passos")
        preview = directory / "visualizacao_animacao.png"
        fig.savefig(preview, dpi=160)
        plt.close(fig)
        output.replace(ROOT / output.name)
        preview.replace(ROOT / preview.name)
    drop = 100 * (1 - float(frames[-1, 0]) / float(frames[0, 0]))
    print(f"Gerados: quadros_difusao.bin ({data.nbytes:,} bytes), visualizacao_animacao.png; "
          f"{count} quadros, queda do pico ≈ {drop:.3f}%.")


if __name__ == "__main__":
    main()
