from __future__ import annotations

import sys
sys.path.append("lib/python")

from visualization.visualize import save_histogram
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parents[1]
LIB_PYTHON = PROJECT_ROOT / "lib" / "python"

if __name__ == "__main__":
    TARGET = PROJECT_ROOT / "data"

    processed = 0
    for csv_path in TARGET.rglob("histogram.csv"):
        save_histogram(csv_path)
        processed += 1
