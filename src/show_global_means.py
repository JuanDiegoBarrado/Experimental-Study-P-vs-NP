"""
plot_medias.py
--------------
Calcula y grafica la evolución de las medias y desviaciones estándar
de histogramas guardados en CSVs (formato bin_left, bin_right, count).

Uso:
    python plot_medias.py carpeta/
    python plot_medias.py carpeta/ --output figura.png
"""

import argparse
import sys
sys.path.append("lib/python")
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

import evaluation.stats as sts


# ---------------------------------------------------------------------------
# Argumentos
# ---------------------------------------------------------------------------

def parse_args():
    parser = argparse.ArgumentParser(
        description="Calcula y grafica la evolución de las medias de histogramas."
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
        default="Evolución de las medias de los histogramas",
        help="Título del gráfico.",
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

    # Gráfico
    fig, ax = plt.subplots(figsize=(12, 6))

    x = range(len(medias))
    medias = np.array(medias)
    desviaciones = np.array(desviaciones)

    # Línea de medias
    ax.plot(x, medias, 'o-', linewidth=2, markersize=6, label='Media')

    # Área sombreada con ±1 desviación estándar
    ax.fill_between(x, medias - desviaciones, medias + desviaciones, 
                     alpha=0.3, label='±1 desv. estándar')

    # Etiquetas y estilo
    ax.set_xlabel('Índice del histograma', fontsize=11)
    ax.set_ylabel('Valor medio', fontsize=11)
    ax.set_title(args.title, fontsize=14, fontweight="bold", pad=12)
    ax.legend(framealpha=0.9)
    ax.spines[["top", "right"]].set_visible(False)
    ax.grid(True, alpha=0.3, linestyle='--')

    fig.tight_layout()

    # Imprimir valores
    print("\nEstadísticas por histograma:")
    for i, (label, m, s) in enumerate(zip(labels, medias, desviaciones)):
        print(f"{label}: media = {m:.2f}, std = {s:.2f}")

    if args.output:
        fig.savefig(args.output, dpi=150, bbox_inches="tight")
        print(f"\nFigura guardada en: {args.output}")
    else:
        plt.show()


if __name__ == "__main__":
    main()