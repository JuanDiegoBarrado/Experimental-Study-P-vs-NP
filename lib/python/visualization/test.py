import pandas as pd
import lib.metrics.metrics as metrics
import matplotlib.pyplot as plt
import numpy as np
from tqdm import tqdm

tqdm.pandas(mininterval=2)

BITS = 32

df = pd.read_csv(
    filepath_or_buffer='dataset.csv',
    usecols=['Funcion', 'Clase'],
    dtype={'Funcion': np.uint64, 'Clase': 'category'}
)   
# df = df.sample(frac=0.75, random_state=42)

metrica = lambda x: metrics.num_classifications(x)
df['sequence'] = df['Funcion'].map(lambda x: format(x, f'0{BITS}b'))
df['measure'] = df['sequence'].progress_apply(metrica)

df.to_csv('jd.csv')

for clase in df['Clase'].unique():
    subset = df[df['Clase'] == clase]
    plt.hist(subset['measure'], bins=20, alpha=0.5, label=clase)

plt.legend()
plt.xlabel('measure')
plt.ylabel('Frecuencia')
plt.title('Distribución de measure por Clase')
plt.show()
