"""
plot_orquillas.py
-----------------
Calcula y grafica las medias de histogramas con barras de error (orquillas)
mostrando la desviación estándar. Las clases se muestran en el eje vertical.

Uso:
    python plot_orquillas.py carpeta/
    python plot_orquillas.py carpeta/ --output figura.png
"""

import argparse
import sys
sys.path.append("lib/python")
from pathlib import Path

import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import numpy as np
import pandas as pd
import evaluation.stats as sts


# ---------------------------------------------------------------------------
# Argumentos
# ---------------------------------------------------------------------------

def parse_args():
    parser = argparse.ArgumentParser(
        description="Grafica medias de histogramas con barras de error horizontales."
    )
    parser.add_argument(
        "metric",
        metavar="METRIC",
        help="Directorio de la métrica en el que buscar los histogramas en ficheros CSV.",
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
        default="Medias y desviaciones estándar por clase",
        help="Título del gráfico.",
    )
    parser.add_argument(
        "--capsize",
        type=float,
        default=5,
        help="Ancho de las tapas de las orquillas (por defecto 5).",
    )
    parser.add_argument(
        "--markersize",
        type=float,
        default=8,
        help="Tamaño de los marcadores (por defecto 8).",
    )
    return parser.parse_args()

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    args = parse_args()

    # Recopilar ficheros
    hists_path = Path("data") / args.metric / "classes"
    if not hists_path.is_dir():
        print(f"ERROR: La métrica '{args.metric}' no es un directorio válido.", file=sys.stderr)
        sys.exit(1)

    pattern = "**/*.csv"
    paths = sorted(hists_path.glob(pattern), key=lambda x: int(x.parent.name.split('_')[1]))

    if not paths:
        print(f"ERROR: no se encontraron CSVs en '{hists_path}'.", file=sys.stderr)
        sys.exit(1)

    # Calcular estadísticas
    medias = []
    desviaciones = []
    labels = []

    for path in paths:
        try:
            df = sts.load_histogram(path)
            mean, std = sts.compute_histogram_statistics(df)
            medias.append(mean)
            desviaciones.append(std)
            labels.append(path.parent.name)
        except Exception as e:
            print(f"AVISO: se omite {path.name} — {e}", file=sys.stderr)
            continue

    if len(medias) == 0:
        print("ERROR: no se pudo cargar ningún CSV válido.", file=sys.stderr)
        sys.exit(1)

    # Convertir a arrays
    medias = np.array(medias)
    desviaciones = np.array(desviaciones)

    # Paleta de colores (la misma que en plot_histogramas.py)
    cmap = plt.get_cmap("tab10")
    colors = [cmap(i % 10) for i in range(len(paths))]

    # Gráfico con ejes intercambiados
    fig, ax = plt.subplots(figsize=(10, max(6, len(labels) * 0.4)))

    # Posiciones en el eje Y (invertido para que la primera clase esté arriba)
    y = np.arange(len(labels))

    # Diagrama de orquillas HORIZONTAL con colores individuales
    for i, (media, desv, label, color) in enumerate(zip(medias, desviaciones, labels, colors)):
        ax.errorbar(
            media, y[i], xerr=desv,
            fmt='o',
            markersize=args.markersize,
            capsize=args.capsize,
            capthick=2,
            linewidth=2,
            elinewidth=1.5,
            color=color,
            ecolor=color,
            markeredgecolor='black',
            markeredgewidth=0.5,
            alpha=0.8
        )

    # Etiquetas y estilo
    ax.set_title(args.title, fontsize=14, fontweight="bold", pad=12)
    ax.set_xlabel("Valor medio", fontsize=11)
    ax.set_ylabel("Clase", fontsize=11)
    ax.set_yticks(y)
    ax.set_yticklabels(labels)
    ax.invert_yaxis()  # Invertir para que la primera clase esté arriba
    ax.xaxis.set_major_formatter(ticker.FuncFormatter(lambda x, _: f"{x:g}"))
    ax.spines[["top", "right"]].set_visible(False)
    ax.grid(axis="x", linestyle="--", alpha=0.4)

    fig.tight_layout()

    # Imprimir valores
    print("\nEstadísticas por clase:")
    for label, m, s in zip(labels, medias, desviaciones):
        print(f"{label}: media = {m:.2f}, std = {s:.2f}")

    if args.output:
        fig.savefig(args.output, dpi=150, bbox_inches="tight")
        print(f"\nFigura guardada en: {args.output}")
    else:
        plt.show()


if __name__ == "__main__":
    main()