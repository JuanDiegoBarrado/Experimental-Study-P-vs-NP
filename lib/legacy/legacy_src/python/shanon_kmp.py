from tqdm import tqdm
import huffman

def match(s: str, pat: str):
    def pi(s: str):
        p = [0] * len(s)
        for i in range(1, len(s)):
            g = p[i-1]
            while (g and s[i] != s[g]):
                g = p[g-1]
            p[i] = g + (s[i] == s[g])
        
        return p
    
    p = pi(pat + '\0' + s)
    res = 0
    for i in range(len(p) - len(s), len(p)):
        if (p[i] == len(pat)):
            res += 1
    return res

MAX_LENGTH = 8

seq = ""
with open('sequence.txt', 'r', encoding='utf-8') as f:
    seq = f.read()

cadenas = [format(i, f'0{MAX_LENGTH}b') for i in range(2**MAX_LENGTH)]
freqs = {cadena: 0.0 for cadena in cadenas}

for elem in freqs:
    freqs[elem] = match(seq, elem) / len(seq)
    
print("Frequencies:")
for elem in freqs:
    print(f"\t'{elem}': {freqs[elem]:.4f}")
print(f"Sum: {sum([freqs[elem] for elem in freqs])}")

codigos = huffman.codebook(freqs.items())
print("Códigos de Huffman:")
for simbolo, codigo in codigos.items():
    print(f"\t'{simbolo}': {codigo}")

trans_seq = [codigos[seq[i:i+MAX_LENGTH]] for i in range(0, len(seq), MAX_LENGTH)]
trans_seq = "".join(trans_seq)
print(f"{len(seq)} -> {len(trans_seq)}: {(len(seq) - len(trans_seq)) / len(seq) * 100:.2f}%")