# boolean_framework.py
import boolean_core
import boolean_functions
import numpy as np
from abc import ABC, abstractmethod
from typing import Callable
import time

class BooleanFunction(ABC):
    """Clase base para funciones booleanas"""
    
    def __init__(self, n_bits: int):
        self.n_bits = n_bits
    
    @abstractmethod
    def get_cpp_function(self):
        """Retorna callable de C++ para máxima eficiencia"""
        pass
    
    def compute_truth_table(self, num_threads=-1, show_progress=False):
        """Computa tabla de verdad usando motor C++"""
        cpp_func = self.get_cpp_function()
        
        print(f"Computing truth table for {self.n_bits} bits (2^{self.n_bits} = {2**self.n_bits:,} evaluations)...")
        start = time.time()
        
        if show_progress:
            def progress(p):
                print(f"\rProgress: {p*100:.1f}%", end='', flush=True)
            
            result = boolean_core.compute_with_progress(
                cpp_func, self.n_bits, progress, num_threads
            )
            print()  # Nueva línea
        else:
            result = boolean_core.compute_truth_table(
                cpp_func, self.n_bits, num_threads
            )
        
        elapsed = time.time() - start
        print(f"Completed in {elapsed:.3f}s ({2**self.n_bits/elapsed:,.0f} eval/s)")
        
        return result
    
    def evaluate(self, x: int) -> bool:
        """Evaluación individual (útil para testing)"""
        return self.get_cpp_function()(x)


class CliqueFunction(BooleanFunction):
    """Función: existe clique de tamaño n/2 en el grafo"""
    
    def get_cpp_function(self):
        return graph_functions.CliqueFunctionWrapper(self.n_bits)


class XORFunction(BooleanFunction):
    """Función: XOR de todos los bits"""
    
    def get_cpp_function(self):
        # Lambda que se ejecuta en C++
        return lambda x: bin(x).count('1') % 2 == 1


class CustomFunction(BooleanFunction):
    """Función personalizada definida en Python (más lenta)"""
    
    def __init__(self, n_bits: int, func: Callable[[int], bool]):
        super().__init__(n_bits)
        self.func = func
    
    def get_cpp_function(self):
        return self.func


# Uso
if __name__ == "__main__":
    # Función simple (rápida)
    print("=== XOR Function ===")
    f_xor = XORFunction(n_bits=20)
    tabla_xor = f_xor.compute_truth_table(num_threads=8)
    print(f"True count: {np.sum(tabla_xor)}")
    
    print("\n=== Clique Function ===")
    # Para grafo de 6 nodos: 6*5/2 = 15 bits
    f_clique = CliqueFunction(n_bits=15)
    tabla_clique = f_clique.compute_truth_table(
        num_threads=8, 
        show_progress=True
    )
    print(f"Graphs with clique of size 3: {np.sum(tabla_clique)}")
    
    # Función custom en Python (desarrollo rápido)
    def mi_funcion(x):
        # Tu lógica aquí
        return (x & 0xFF) > 127
    
    f_custom = CustomFunction(n_bits=16, func=mi_funcion)
    tabla_custom = f_custom.compute_truth_table()