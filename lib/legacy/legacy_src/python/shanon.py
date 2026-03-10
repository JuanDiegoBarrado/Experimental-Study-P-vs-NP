from tqdm import tqdm
import huffman

MAX_LENGTH = 8

seq = ""
with open('legacy/sequence.txt', 'r', encoding='utf-8') as f:
    seq = f.read()

cadenas = [format(i, f'0{MAX_LENGTH}b') for i in range(2**MAX_LENGTH)]
freqs = {cadena: 0.0 for cadena in cadenas}

trans_seq = [freqs[seq[i:i+MAX_LENGTH]] for i in range(0, len(seq), MAX_LENGTH)]

TOTAL = len(seq) / MAX_LENGTH
for i in range(0, len(seq), MAX_LENGTH):
    freqs[seq[i:i+MAX_LENGTH]] += 1
for cadena in freqs:
    freqs[cadena] /= TOTAL
    
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