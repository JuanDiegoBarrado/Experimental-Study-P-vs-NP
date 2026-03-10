from itertools import combinations
from tqdm import tqdm

def distincts_subchains_of_size(sequence, size):
    subchains = set([''.join(c) for c in combinations(sequence, size)])
    return len(subchains)

def jd_solve(seq, k):
    n = len(seq)
    dp = [[0 for _ in range(k+1)] for _ in range(n+1)]
    terminan = [[[0, 0] for _ in range(k+1)] for _ in range(n+1)]
    
    for i in range(n+1):
        for j in range(i+1, k+1):
            dp[i][i] = 0
    for i in range(n+1):
        dp[i][0] = 1
    
    for i in range(1, n+1):
        char = int(seq[i-1])
        no_char = 1 - int(seq[i-1])
        for j in range(1, min(i+1, k+1)):
            dp[i][j] = dp[i-1][j] + (dp[i-1][j-1] - terminan[i-1][j][char])
            terminan[i][j][char] = terminan[i-1][j][char] + (dp[i-1][j-1]- terminan[i-1][j][char])
            terminan[i][j][no_char] = dp[i][j] - terminan[i][j][char]
            
    return dp[n][k]

BITS = 16
for i in tqdm(range(2**BITS)):
    seq = format(i, f'0{BITS}b')
    for k in range(len(seq)):
        jd = jd_solve(seq, k)
        ds = distincts_subchains_of_size(seq, k)
        if jd != ds:
            print(f"NO({k}) -> {seq}: jd {jd}, ds {ds}")