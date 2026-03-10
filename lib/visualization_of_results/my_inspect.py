import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv('jd.csv')

for clase in df['Clase'].unique():
    subset = df[df['Clase'] == clase]
    plt.hist(subset['measure'], bins=1000, alpha=0.5, label=clase)

plt.legend()
plt.xlabel('measure')
plt.ylabel('Frecuencia')
plt.title('Distribución de measure por Clase')
plt.show()