import pandas as pd
import matplotlib.pyplot as plt

# Leer el archivo CSV
df = pd.read_csv('table.csv')

# Identificar las columnas de aristas (todas menos 'f')
columnas_aristas = [col for col in df.columns if col.startswith('arista_')]

# Calcular el conteo para cada arista
conteos: dict[str, int] = {}
for arista in columnas_aristas:
    # Contar filas donde la arista es 1 Y f es 1
    conteos[arista] = ((df[arista] == 1) & (df['f'] == 1)).sum()

# Crear el gráfico de barras
plt.figure(figsize=(10, 6))
plt.bar(list(conteos.keys()), list(conteos.values()), color='steelblue', edgecolor='black')
plt.xlabel('Aristas', fontsize=12)
plt.ylabel('Número de coincidencias con f=1', fontsize=12)
plt.title('Conteo de aristas cuando f=1', fontsize=14, fontweight='bold')
plt.xticks(rotation=45, ha='right')
plt.tight_layout()
plt.grid(axis='y', alpha=0.3)
plt.show()