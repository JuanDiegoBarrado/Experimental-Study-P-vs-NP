from sys import stdin

lines = [x.strip().split() for x in stdin]
d = {int(l[0].split(':')[0]) : tuple(int(x) for x in l[1:]) for l in lines if len(l) >= 2}
print(d)

a, b, c = 10, 11, 12

assert(d[0] == (2, 12))
assert(d[1] == (2, 100))
assert(d[2] == (5, 1140))
assert(d[3] == (20, 11570))
assert(d[4] == (93, 109826))
assert(d[5] == (389, 995240))
assert(d[6] == (1988, 8430800))
assert(d[7] == (11382, 63401728))
assert(d[8] == (60713, 383877392))
assert(d[9] == (221541, 1519125536))
assert(d[a] == (293455, 2123645248))
assert(d[b] == (26535, 195366784))
assert(d[c] == (1, 1920))

print("Table agrees with chapter 7.1.2 of Knuth")
