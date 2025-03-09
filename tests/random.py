
seed = 0

for i in range(32):
    seed = (seed * 1103515245 + 12345) & 0xffffffff
    print(seed)