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
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


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
# Cálculo de estadísticas
# ---------------------------------------------------------------------------

def calcular_estadisticas_histograma(df: pd.DataFrame):
    """Calcula media y desviación estándar de un histograma."""
    # Punto medio de cada bin
    bin_centers = (df['bin_left'] + df['bin_right']) / 2
    
    # Media ponderada
    total_count = df['count'].sum()
    mean = (bin_centers * df['count']).sum() / total_count
    
    # Desviación estándar ponderada
    variance = ((bin_centers - mean) ** 2 * df['count']).sum() / total_count
    std = np.sqrt(variance)
    
    return mean, std


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
            df = load_csv(path)
            mean, std = calcular_estadisticas_histograma(df)
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