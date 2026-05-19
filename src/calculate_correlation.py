import sys
sys.path.append("lib/python")

from pathlib import Path
import pandas as pd
import argparse
import evaluation.stats as sts

NUM_CLASSES = 13

def load_csv(path: Path) -> pd.DataFrame:
    df = pd.read_csv(path)
    required = {"bin_left", "bin_right", "count"}
    if not required.issubset(df.columns):
        raise ValueError(
            f"{path.name}: se esperan columnas {required}, "
            f"pero se encontraron {set(df.columns)}"
        )
    return df

def parse_args():
    parser = argparse.ArgumentParser(
        description="Construye la matriz de confusión de la métrica para calcular la correlación."
    )
    parser.add_argument(
        "metric",
        metavar="METRIC",
        help="Métrica sobre la que calcular la correlación.",
    )
    return parser.parse_args()


def main():
    args = parse_args()

    # Recopilar ficheros
    base = Path("data/" / args.metric / "classes")
    if not base.is_dir():
        print(f"ERROR: '{base}' no es un directorio válido.", file=sys.stderr)
        sys.exit(1)

    csv_paths = sorted(base.glob("**/*.csv"), key=lambda x: int(x.parent.name.split('_')[1]))
    min_paths = sorted(base.glob("**/min_score.txt"), key=lambda x: int(x.parent.name.split('_')[1]))
    max_paths = sorted(base.glob("**/max_score.txt"), key=lambda x: int(x.parent.name.split('_')[1]))

    if not csv_paths or not min_paths or not max_paths:
        print(f"ERROR: no se encontraron CSVs en '{base}'.", file=sys.stderr)
        sys.exit(1)
    
    min_value = None
    for p in min_paths:
        with open(p, "r", encoding="utf-8") as f:
            val = int(f.read())
            min_value = val if min_value is None else min(min_value, val)
    max_value = None
    for p in max_paths:
        with open(p, "r", encoding="utf-8") as f:
            val = int(f.read())
            max_value = val if max_value is None else max(max_value, val)
    
    if min_value is None or max_value is None:
        print(f"ERROR: no se encontraron CSVs en '{base}'.", file=sys.stderr)
        sys.exit(1)
    
    matrix = [[0 for _ in range(max_value - min_value + 1)] for _ in range(NUM_CLASSES)]
    for i in range(len(matrix)):
        df = load_csv(csv_paths[i])
        for _, row in df.iterrows():
            bin_idx = row["bin_left"]
            count = row["count"]
            matrix[i][bin_idx] = int(count)
    
    
    # Uso con tu matriz:
    pearson = sts.pearson_correlation(matrix)
    tau_b = sts.kendall_tau_b(matrix)
    tau_c = sts.kendall_tau_c(matrix)
    print(f"Pearson: {pearson:.4f}")
    print(f"Tau-b de Kendall: {tau_b:.4f}")
    print(f"Tau-c de Kendall: {tau_c:.4f}")
                        
    
if __name__ == "__main__":
    main()