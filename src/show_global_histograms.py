"""
plot_histogramas.py
-------------------
Combina histogramas guardados en CSVs (formato bin_left, bin_right, count)
en una única figura, coloreando cada histograma según su fichero de origen.

Uso:
    python plot_histogramas.py carpeta/
    python plot_histogramas.py carpeta/ --recursive         # busca también en subcarpetas
    python plot_histogramas.py carpeta/ --output figura.png
"""

import argparse
import sys
from pathlib import Path

import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import pandas as pd


# ---------------------------------------------------------------------------
# Argumentos
# ---------------------------------------------------------------------------

def parse_args():
    parser = argparse.ArgumentParser(
        description="Superpone histogramas de varios CSVs en una sola figura."
    )
    parser.add_argument(
        "dir",
        metavar="CARPETA",
        help="Directorio en el que buscar ficheros CSV.",
    )
    parser.add_argument(
        "--recursive", "-r",
        action="store_true",
        help="Busca CSVs también dentro de subcarpetas.",
    )
    parser.add_argument(
        "--alpha",
        type=float,
        default=0.55,
        help="Transparencia de las barras (0-1, por defecto 0.55).",
    )
    parser.add_argument(
        "--normalize",
        action="store_true",
        help="Normaliza cada histograma a densidad de probabilidad.",
    )
    parser.add_argument(
        "--output",
        metavar="FICHERO",
        default=None,
        help="Guarda la figura en este fichero (p.ej. salida.png). "
             "Si no se indica, se muestra en pantalla.",
    )
    parser.add_argument(
        "--title",
        default="Comparativa de distribuciones",
        help="Título del gráfico.",
    )
    return parser.parse_args()


# ---------------------------------------------------------------------------
# Carga de datos
# ---------------------------------------------------------------------------

def load_csv(path: Path) -> pd.DataFrame:
    df = pd.read_csv(path)
    required = {"bin_left", "bin_right", "count"}
    if not required.issubset(df.columns):
        raise ValueError(
            f"{path.name}: se esperan columnas {required}, "
            f"pero se encontraron {set(df.columns)}"
        )
    return df


# ---------------------------------------------------------------------------
# Dibujo
# ---------------------------------------------------------------------------

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


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    args = parse_args()

    # Recopilar ficheros
    base = Path(args.dir)
    if not base.is_dir():
        print(f"ERROR: '{base}' no es un directorio válido.", file=sys.stderr)
        sys.exit(1)

    pattern = "**/*.csv" if args.recursive else "*.csv"
    paths = sorted(base.glob(pattern), key=lambda x: int(x.parent.name.split('_')[1]))

    if not paths:
        print(f"ERROR: no se encontraron CSVs en '{base}'.", file=sys.stderr)
        sys.exit(1)

    # Paleta de colores (cíclica si hay muchos ficheros)
    cmap = plt.get_cmap("tab10")
    colors = [cmap(i % 10) for i in range(len(paths))]

    fig, ax = plt.subplots(figsize=(10, 5))

    loaded = 0
    for path, color in zip(paths, colors):
        try:
            df = load_csv(path)
        except Exception as e:
            print(f"AVISO: se omite {path.name} — {e}", file=sys.stderr)
            continue

        draw_histogram(
            ax, df,
            label=path.parent.name,
            color=color,
            alpha=args.alpha,
            normalize=args.normalize,
        )
        loaded += 1

    if loaded == 0:
        print("ERROR: no se pudo cargar ningún CSV válido.", file=sys.stderr)
        sys.exit(1)

    # Etiquetas y estilo
    ax.set_title(args.title, fontsize=14, fontweight="bold", pad=12)
    ax.set_xlabel("Valor")
    ax.set_ylabel("Densidad de probabilidad" if args.normalize else "Frecuencia")
    ax.yaxis.set_major_formatter(ticker.FuncFormatter(lambda x, _: f"{x:g}"))
    ax.legend(title="Fichero", framealpha=0.9)
    ax.spines[["top", "right"]].set_visible(False)
    ax.grid(axis="y", linestyle="--", alpha=0.4)

    fig.tight_layout()

    if args.output:
        fig.savefig(args.output, dpi=150, bbox_inches="tight")
        print(f"Figura guardada en: {args.output}")
    else:
        plt.show()


if __name__ == "__main__":
    main()