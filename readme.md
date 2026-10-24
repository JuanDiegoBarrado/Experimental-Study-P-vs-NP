<div align="center">

# 🧠 Predicción del Tamaño de Circuitos Mínimos
**Estudio experimental sobre la repetitividad de funciones booleanas y la complejidad de circuitos (P vs NP)**

[![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat-square)](https://isocpp.org/)
[![Python](https://img.shields.io/badge/Python-3.x-yellow.svg?style=flat-square)](https://www.python.org/)
[![Memoria](https://img.shields.io/badge/Memoria-PDF-red.svg?style=flat-square)](docs/memoria_TFG.pdf)
[![Code License: MIT](https://img.shields.io/badge/Código-MIT-green.svg?style=flat-square)](LICENSE)
[![Docs License: CC BY-NC-ND 4.0](https://img.shields.io/badge/Memoria-CC%20BY--NC--ND%204.0-lightgrey.svg?style=flat-square)](docs/LICENSE)

</div>

---

## 📄 Memoria del TFG

Este repositorio contiene el código asociado al Trabajo de Fin de Grado **«Predicción del tamaño de circuitos mínimos a través de la repetitividad de funciones booleanas»** (*Prediction of minimum circuit size through repetitiveness of Boolean functions*), defendido el 10 de junio de 2026 y calificado con un **10**. La memoria completa está disponible en [`docs/memoria_TFG.pdf`](docs/memoria_TFG.pdf).

La memoria aborda el problema desde dos vertientes:
- **Teórica**: exposición de las mejores cotas inferiores conocidas para el tamaño de circuitos sobre la base $B_2$ mediante la técnica de eliminación de puertas aplicada a los *affine dispersers* (de $3n - o(n)$ a $3{,}1n - o(n)$).
- **Experimental**: propuesta y evaluación de métricas basadas en la entropía de Shannon, el número de clasificaciones y la **Forma Normal de Fourier (FNF)**. En funciones de 5 bits, la métrica FNF alcanza una correlación de **0,4463** con el tamaño del circuito mínimo, respetando en media su monotonía.

Si utilizas este trabajo, puedes citarlo como:

```bibtex
@thesis{barrado2026tfg,
  author = {Barrado Daganzo, Juan Diego},
  title  = {Predicción del tamaño de circuitos mínimos a través de la repetitividad de funciones booleanas},
  type   = {Trabajo de Fin de Grado},
  school = {Universidad Complutense de Madrid},
  year   = {2026},
  url    = {https://github.com/JuanDiegoBarrado/Experimental-Study-P-vs-NP}
}
```

---

## 🎯 Motivación del Proyecto

El problema **P vs NP** es uno de los mayores desafíos abiertos en la informática teórica. Una de las estrategias más prometedoras para abordarlo es intentar separar estas clases demostrando que los problemas en NP requieren circuitos booleanos de tamaño superpolinómico. Sin embargo, las demostraciones teóricas tradicionales (como la técnica de eliminación de puertas) se enfrentan a obstáculos formales (como las *demostraciones naturales*) y limitaciones severas para superar cotas lineales.

Este proyecto propone una **metodología alternativa y puramente experimental**. En lugar de intentar probar teóricamente cotas inferiores, exploramos si podemos **predecir el tamaño del circuito mínimo** de una función booleana analizando exclusivamente la regularidad y repetitividad (evidente o subyacente) en su **tabla de verdad**. 

Si encontramos una métrica computable que correlacione perfectamente con el tamaño del circuito y demuestre que ciertas funciones son incompresibles, podríamos abrir una nueva vía para encontrar funciones explícitas con circuitos de complejidad exponencial, arrojando luz sobre el problema de P vs NP.

---

## ⚙️ Cómo Funciona (Programas Principales)

El repositorio está compuesto por una librería en C++ de alto rendimiento para manipular funciones booleanas y grafos, junto con un conjunto de ejecutables y scripts en Python para el análisis estadístico. Los programas más relevantes ubicados en `src/` incluyen:

### 🛠️ Métricas y Análisis
- **`calculate_metric_stochastic_by_classes.cpp`**: Calcula métricas de complejidad basadas en repetitividad de subsecuencias o entropía de Shannon sobre funciones booleanas.
- **`calculate_min_expression.cpp`**: Evalúa expresiones mínimas (FND/FNC) para intentar correlacionar la redundancia sintáctica con la complejidad del circuito.
- **`calculate_fnf_expression.cpp`**: Computa la *Forma Normal de Fourier* (FNF) generalizada para capturar el solapamiento de patrones (armónicos booleanos fundamentales) de forma subyacente en la tabla de verdad.

### 🧩 Optimalidad y Circuitos
- **`optimal5_binary.cpp`** / **`optimal5_binary_classes.cpp`**: Evalúan y comparan funciones booleanas de hasta 5 variables, buscando circuitos óptimos de forma exhaustiva para establecer una "verdad base" (ground truth).
- **`optimal5_numgates.cpp`**: Herramienta de apoyo para el cálculo de la complejidad exacta de funciones en base a su número de puertas.

### 📊 Visualización (Python)
Dentro de la carpeta `src/` encontrarás herramientas en Python para extraer conclusiones de los datos generados:
- **`calculate_correlation.py`**: Calcula la correlación entre las métricas propuestas y el tamaño óptimo real del circuito.
- **`show_global_histograms.py`** / **`show_global_means.py`**: Generan gráficos e histogramas para visualizar cómo se distribuyen las métricas por clases de complejidad.

---

## 🚀 Cómo Compilar e Instalar

Para clonar y ejecutar tus propios experimentos, necesitarás un compilador de C++ (GCC/Clang) y `make` para construir los binarios, además de Python para los scripts de visualización.

### 1. Requisitos Previos
- **C++**: Compilador que soporte el estándar C++ (ej. `g++`).
- **Make**: Para orquestar la compilación.
- **Python 3**: Para ejecutar los scripts de visualización. (Instalar dependencias con pip si fuese necesario para matplotlib/numpy).

### 2. Compilación
El proyecto incluye un `Makefile` en la raíz. Para compilar todos los programas principales en C++, simplemente abre una terminal en la raíz del proyecto y ejecuta:

```bash
make all
```

> **Tip**: Si deseas habilitar los flags de depuración, utiliza `make DEBUG=1`. Para compilación estática, usa `make STATIC=1`.

Esto generará la carpeta `build/` con todos los ejecutables compilados (`.exe`).

### 3. Ejecución de Pruebas
Puedes probar los binarios generados pasándoles los parámetros correspondientes. Por ejemplo:

```bash
./build/calculate_metric_stochastic_by_classes.exe --metric num_classifications --class 9 --batch 100000 --prob 0.15
```

### 4. Generación de Gráficas
Para ejecutar los scripts de Python, se recomienda crear un entorno virtual e instalar dependencias (en Windows usando PowerShell):

```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
pip install -r requirements.txt
python src/show_global_histograms.py
```

*(Asegúrate de haber generado previamente los datos en la carpeta `data/` o de tenerlos disponibles).*

---

## 📂 Estructura del Proyecto

- `src/`: Código fuente de los ejecutables y scripts de experimentación.
- `lib/`: Librerías core en C++ (`boolean_functions`, `graph_manipulations`, `metrics`, `visualization`).
- `third_party/`: Dependencias externas y herramientas de terceros.
- `build/`: Directorio donde se guardan los binarios tras la compilación.
- `test/`: Batería de pruebas.
- `docs/`: Memoria del Trabajo de Fin de Grado en PDF.

---

## ⚖️ Licencia

- **Código**: [MIT](LICENSE).
- **Memoria** (`docs/memoria_TFG.pdf`): [CC BY-NC-ND 4.0](docs/LICENSE). Puede compartirse citando al autor, sin fines comerciales y sin obras derivadas. Las figuras reproducidas de otros trabajos conservan los derechos de sus autores.
- `data/`: Ficheros generados durante los estudios experimentales (historiales, tablas, métricas precalculadas).

<br>
<div align="center">
  <i>Trabajo de Fin de Grado en el Doble Grado en Ingeniería Informática y Matemáticas (UCM) <br> Autor: Juan Diego Barrado Daganzo <br> Directores: Ismael Rodríguez Laguna y Narciso Martí Oliet</i>
</div>