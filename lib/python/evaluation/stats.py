import math
import pandas as pd
from pathlib import Path

def kendall_tau_b(matrix):
    """
    Calcula la tau-b de Kendall para una tabla de contingencia.
    
    Args:
        matrix: matriz de confusión (numpy array) donde filas=X, columnas=Y
        
    Returns:
        tau_b: coeficiente tau-b de Kendall
    """
    n_rows, n_cols = len(matrix), len(matrix[0])
    n = int(sum([sum(row) for row in matrix]))
    
    # Calcular pares concordantes (P)
    P = 0
    for i in range(n_rows):
        for j in range(n_cols):
            # Sumar todos los elementos abajo y a la derecha
            P += matrix[i][j] * sum(matrix[row][col] for row in range(i+1, len(matrix)) for col in range(j+1, len(matrix[0])))
    
    # Calcular pares discordantes (Q)
    Q = 0
    for i in range(n_rows):
        for j in range(n_cols):
            # Sumar todos los elementos abajo y a la izquierda
            Q += matrix[i][j] * sum(matrix[row][col] for row in range(i+1, len(matrix)) for col in range(j))
    
    # Calcular empates en filas (mismo X, diferente Y)
    T_x = 0
    for i in range(n_rows):
        row_sum = sum(matrix[i])
        T_x += row_sum * (row_sum - 1) / 2
    
    # Calcular empates en columnas (mismo Y, diferente X)
    T_y = 0
    for j in range(n_cols):
        col_sum = sum(row[j] for row in matrix)
        T_y += col_sum * (col_sum - 1) / 2
    
    # Calcular tau-b
    numerator = P - Q
    denominator = math.sqrt((n * (n - 1) / 2 - T_x) * (n * (n - 1) / 2 - T_y))
    
    if denominator == 0:
        return 0.0
    
    tau_b = numerator / denominator
    
    return tau_b


def kendall_tau_c(matrix):
    """
    Calcula la tau-b de Kendall para una tabla de contingencia.
    
    Args:
        matrix: matriz de confusión (numpy array) donde filas=X, columnas=Y
        
    Returns:
        tau_b: coeficiente tau-b de Kendall
    """
    n_rows, n_cols = len(matrix), len(matrix[0])
    n = int(sum([sum(row) for row in matrix]))
    m = int(min(n_rows, n_cols))
    
    # Calcular pares concordantes (P)
    P = 0
    for i in range(n_rows):
        for j in range(n_cols):
            # Sumar todos los elementos abajo y a la derecha
            P += matrix[i][j] * sum(matrix[row][col] for row in range(i+1, len(matrix)) for col in range(j+1, len(matrix[0])))
    
    # Calcular pares discordantes (Q)
    Q = 0
    for i in range(n_rows):
        for j in range(n_cols):
            # Sumar todos los elementos abajo y a la izquierda
            Q += matrix[i][j] * sum(matrix[row][col] for row in range(i+1, len(matrix)) for col in range(j))
    
    # Calcular tau-c
    numerator = 2 * (P - Q) * m
    denominator = n * n * (m - 1)
    
    if denominator == 0:
        return 0.0
    
    tau_c = numerator / denominator
    return tau_c


def pearson_correlation(matrix):
    """
    Calcula la correlación de Pearson para una tabla de contingencia.
    Sin dependencias de NumPy.

    Args:
        matrix: Lista de listas de enteros (tabla de contingencia).

    Returns:
        r: Coeficiente de correlación de Pearson (float).
    """
    n_rows = len(matrix)
    if n_rows == 0:
        return 0.0
    n_cols = len(matrix[0])

    # 1. Totales por fila, columna y gran total (n)
    row_totals = [sum(row) for row in matrix]
    col_totals = [sum(matrix[r][c] for r in range(n_rows)) for c in range(n_cols)]
    n = float(sum(row_totals))

    if n == 0:
        return 0.0

    # 2. Cálculo de Esperanzas (Medias de los índices ponderadas)
    # E[X] y E[Y]
    mean_x = sum(i * row_totals[i] for i in range(n_rows)) / n
    mean_y = sum(j * col_totals[j] for j in range(n_cols)) / n

    # 3. Cálculo de Esperanzas de los cuadrados (E[X^2] y E[Y^2])
    ex2 = sum((i**2) * row_totals[i] for i in range(n_rows)) / n
    ey2 = sum((j**2) * col_totals[j] for j in range(n_cols)) / n

    # 4. Cálculo de la Esperanza del producto (E[XY])
    # Equivale a sum(matrix * np.outer(row_values, col_values)) / n
    exy = 0.0
    for i in range(n_rows):
        for j in range(n_cols):
            exy += matrix[i][j] * i * j
    exy /= n

    # 5. Varianzas y Covarianza
    var_x = ex2 - (mean_x ** 2)
    var_y = ey2 - (mean_y ** 2)

    # Si no hay variación en alguna variable, la correlación es 0
    if var_x <= 1e-15 or var_y <= 1e-15:
        return 0.0

    covariance = exy - (mean_x * mean_y)
    
    # 6. Coeficiente de Pearson
    r = covariance / math.sqrt(var_x * var_y)

    # Clip manual para asegurar que esté en el rango [-1.0, 1.0]
    return max(-1.0, min(1.0, r))

def compute_histogram_statistics(df: pd.DataFrame):
    """Calcula media y desviación estándar de un histograma."""
    # Punto medio de cada bin
    bin_centers = (df['bin_left'] + df['bin_right']) / 2
    
    # Media ponderada
    total_count = df['count'].sum()
    mean = (bin_centers * df['count']).sum() / total_count
    
    # Desviación estándar ponderada
    variance = ((bin_centers - mean) ** 2 * df['count']).sum() / total_count
    std = math.sqrt(variance)
    
    return mean, std

def load_histogram(path: Path) -> pd.DataFrame:
    df = pd.read_csv(path)
    required = {"bin_left", "bin_right", "count"}
    if not required.issubset(df.columns):
        raise ValueError(
            f"{path.name}: se esperan columnas {required}, "
            f"pero se encontraron {set(df.columns)}"
        )
    return df