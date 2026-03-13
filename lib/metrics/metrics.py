import huffman
from sympy.logic.boolalg import Or, And, Not, simplify_logic
from sympy import symbols
import numpy as np

def num_ones(sequence: str):
    return sequence.count('1')

def shanon_compression(sequence: str, chunk_size) -> float:
    # Calculate all possible chunks of length
    possible_chunks = [format(i, f'0{chunk_size}b') for i in range(2**chunk_size)]
    chunks_frequency = {chunk: 0.0 for chunk in possible_chunks}

    # Calculate each chunk's frequency based on the given sequence
    TOTAL = len(sequence) / chunk_size
    for i in range(0, len(sequence), chunk_size):
        chunks_frequency[sequence[i:i+chunk_size]] += 1
    for cadena in chunks_frequency:
        chunks_frequency[cadena] /= TOTAL

    codigos = huffman.codebook(chunks_frequency.items())
    translated_sequence = [codigos[sequence[i:i+chunk_size]] for i in range(0, len(sequence), chunk_size)]
    translated_sequence = "".join(translated_sequence)
    
    return (len(sequence) - len(translated_sequence)) / len(sequence)

def fourier_descomposition(sequence: str):
    def minterm_to_expr(index):
        bin_str = format(index, f'0{n}b')
        term = [variables[i] if bit == '1' else Not(variables[i])
                for i, bit in enumerate(bin_str)]
        return And(*term)
    
    def get_literals(minterm):
        """Retorna los literales de un mintérmino (And de literales)"""
        if minterm.func == And:
            return set(minterm.args)
        else:
            return {minterm}

    def literal_order(literal):
        """Retorna clave de orden: (índice_variable, es_negada)
        x4=(-4,0), ~x4=(-4,1), x3=(-3,0), ~x3=(-3,1), ...
        Mayor índice = mayor prioridad, positiva > negada
        """
        if literal.func == Not:
            var = literal.args[0]
            return (-int(var.name[1:]), 1)
        else:
            return (-int(literal.name[1:]), 0)
        
    class tree:
        root: object
        left: "tree"
        right: "tree"
        
        def __init__(self, root, left, right):
            self.root=root
            self.left=left
            self.right=right
            
        def to_string(self):
            var = self.root
            left_tree = self.left.to_string() if self.left is not None else ''
            right_tree = self.right.to_string() if self.right is not None else ''
            
            left_tree = '' if left_tree == '' else f"^({left_tree})"
            right_tree = '' if right_tree == '' else f" + {right_tree}"
            
            return f"{var}{left_tree}{right_tree}"
    
    def decompose(expr):
        if expr == True or expr == False:
            return tree(root=True, left=None, right=None)
        if expr.is_Atom or (expr.func == Not and expr.args[0].is_Atom):
            return tree(root=expr, left=None, right=None)
        if expr.func == And:
            sorted_args = sorted(get_literals(expr), key=literal_order)
            remaining = sorted_args[1:]
            return tree(root=sorted_args[0], left=decompose(And(*remaining)), right=None)
        elif expr.func == Or:
            subexpr = list(expr.args)
            
            all_literals = sorted(
                {lit for subexpression in subexpr for lit in get_literals(subexpression)},
                key=literal_order
            )
            geq_var = all_literals[0]  # El de mayor prioridad según el orden
            
            # expr_vars = sorted({var for subexpression in subexpr for var in subexpression.free_symbols}, key=lambda x: int(x.name[1:]))
            # geq_var = expr_vars[-1]

            subexpr_with_geq_var = []
            subexpr_without_geq_var = []
            for minterm in subexpr:
                minterm_literals = get_literals(minterm)
                if geq_var in minterm_literals:
                    new_factors = minterm_literals - {geq_var}
                    subexpr_with_geq_var.append(And(*new_factors))
                else:
                    subexpr_without_geq_var.append(minterm)

            part_with = Or(*subexpr_with_geq_var) if len(subexpr_with_geq_var) > 0 else 0
            part_without = Or(*subexpr_without_geq_var) if len(subexpr_without_geq_var) > 0 else 0

            # Recursive decomposition
            if part_with != 0 and part_without != 0:    
                iz = decompose(part_with) 
                dc = decompose(part_without)
                return tree(root=geq_var, left=iz, right=dc)
            elif part_with == 0 and part_without != 0:
                dc = decompose(part_without)
                return tree(root=geq_var, left=None, right=dc)
            elif part_without != 0 and part_without == 0:
                iz = decompose(part_with) 
                return tree(root=geq_var, left=iz, right=None)
            else:
                return tree(root=geq_var, left=None, right=None)

    n = int(np.log2(len(sequence)))
    variables = symbols(f'x0:{n}')  # x0, x1, ..., x(n-1)
    
    minterms = [i for i, val in enumerate(sequence) if val == '1']
    # Build Boolean function
    if not minterms:
        return 0
    else:
        f = Or(*(minterm_to_expr(i) for i in minterms))
    
    # Minimize
    f_min = simplify_logic(f, form='dnf')

    return decompose(f_min).to_string()

def vc_values(sequence: str):
    n = len(sequence)
    k = n
    dp = [[0 for _ in range(k+1)] for _ in range(n+1)]
    terminan = [[[0, 0] for _ in range(k+1)] for _ in range(n+1)]
    
    for i in range(n+1):
        for j in range(i+1, k+1):
            dp[i][i] = 0
    for i in range(n+1):
        dp[i][0] = 1
    
    for i in range(1, n+1):
        char = int(sequence[i-1])
        no_char = 1 - int(sequence[i-1])
        for j in range(1, min(i+1, k+1)):
            dp[i][j] = dp[i-1][j] + (dp[i-1][j-1] - terminan[i-1][j][char])
            terminan[i][j][char] = terminan[i-1][j][char] + (dp[i-1][j-1]- terminan[i-1][j][char])
            terminan[i][j][no_char] = dp[i][j] - terminan[i][j][char]
    
    return sum(dp[n])