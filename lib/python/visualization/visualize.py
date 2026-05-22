import csv
import matplotlib.pyplot as plt
from pathlib import Path
import pandas as pd
import numpy as np

def save_histogram(csv_file):
    csv_path = Path(csv_file)

    with csv_path.open(newline="", encoding="utf-8") as fh:
        reader = csv.DictReader(fh)
        fieldnames = set(reader.fieldnames or [])

        required_cols = {"bin_left", "bin_right", "count"}
        missing_cols = required_cols.difference(fieldnames)
        if missing_cols:
            missing = ", ".join(sorted(missing_cols))
            raise ValueError(f"Faltan columnas en el CSV: {missing}")

        rows = [
            (
                float(row["bin_left"]),
                float(row["bin_right"]),
                float(row["count"]),
            )
            for row in reader
        ]

    if not rows:
        raise ValueError(f"CSV vacío: {csv_path}")

    rows.sort(key=lambda row: row[0])
    left = [row[0] for row in rows]
    right = [row[1] for row in rows]
    counts = [row[2] for row in rows]
    widths = [r - l for l, r in zip(left, right)]

    fig, ax = plt.subplots(figsize=(10, 5))
    ax.bar(left, counts, width=widths, align="edge", alpha=0.85, edgecolor="black", linewidth=0.2)

    ax.set_xlabel("measure")
    ax.set_ylabel("Frecuencia")
    ax.set_title("Histograma de measure (desde CSV)")
    ax.set_xlim(left[0], right[-1])
    ax.ticklabel_format(axis="y", style="sci", scilimits=(0, 0))

    plt.tight_layout()
    output_path = csv_path.with_name("histogram.png")
    fig.savefig(output_path, dpi=200, bbox_inches="tight")
    plt.close(fig)

    return output_path


def draw_histogram(ax, df: pd.DataFrame, label: str, color, alpha: float, normalize: bool):
    lefts  = df["bin_left"].to_numpy(dtype=float)
    rights = df["bin_right"].to_numpy(dtype=float)
    counts = df["count"].to_numpy(dtype=float)

    # Los bins son enteros con intervalos cerrados [bin_left, bin_right].
    # El ancho real de cada bin es bin_right - bin_left + 1.
    # Las barras se posicionan de (bin_left - 0.5) a (bin_right + 0.5)
    # para que cada entero quede centrado en su barra, con un pequeño
    # hueco visual entre barras contiguas.
    gap = 0.05
    bar_lefts = lefts - 0.5 + gap / 2
    widths    = (rights - lefts + 1) - gap

    if normalize:
        real_widths = rights - lefts + 1   # ancho real para el área
        total_area = np.sum(counts * real_widths)
        heights = counts / total_area if total_area > 0 else counts
    else:
        heights = counts

    ax.bar(
        bar_lefts,
        heights,
        width=widths,
        align="edge",
        label=label,
        color=color,
        alpha=alpha,
        edgecolor=color,
        linewidth=0.8,
    )