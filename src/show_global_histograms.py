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
sys.path.append("lib/python")
from pathlib import Path

import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import evaluation.stats as sts
import visualization.visualize as visual


# ---------------------------------------------------------------------------
# Argumentos
# ---------------------------------------------------------------------------

def parse_args():
    parser = argparse.ArgumentParser(
        description="Superpone histogramas de varios CSVs en una sola figura."
    )
    parser.add_argument(
        "metric",
        metavar="METRIC",
        help="Directorio de la métrica en el que buscar los histogramas en ficheros CSV.",
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
# Main
# ---------------------------------------------------------------------------

def main():
    args = parse_args()

    # Recopilar ficheros
    hists_path = "data" / Path(args.metric) / "classes"
    if not hists_path.is_dir():
        print(f"ERROR: La métrica '{args.metric}' no es un directorio válido.", file=sys.stderr)
        sys.exit(1)

    pattern = "**/*.csv"
    paths = sorted(hists_path.glob(pattern), key=lambda x: int(x.parent.name.split('_')[1]))

    if not paths:
        print(f"ERROR: no se encontraron CSVs en '{hists_path}'.", file=sys.stderr)
        sys.exit(1)

    # Paleta de colores (cíclica si hay muchos ficheros)
    cmap = plt.get_cmap("tab10")
    colors = [cmap(i % 10) for i in range(len(paths))]

    fig, ax = plt.subplots(figsize=(10, 5))

    loaded = 0
    for hists_path, color in zip(paths, colors):
        try:
            df = sts.load_histogram(hists_path)
        except Exception as e:
            print(f"AVISO: se omite {hists_path.name} — {e}", file=sys.stderr)
            continue

        visual.draw_histogram(
            ax, df,
            label=hists_path.parent.name,
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