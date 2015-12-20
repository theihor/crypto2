#!/usr/bin/python3
import sys

args = sys.argv[1:]
f_name = args[0]

f = open(f_name, "rb")
stats = {}
for i in range(16):
    stats[i] = 0
try:
    byte = f.read(1)
    while byte:
        i = int.from_bytes(byte, byteorder='little')
        l = i >> 4
        r = i & 0x0f
        stats[l] += 1
        stats[r] += 1
        
        byte = f.read(1)
finally:
    #print(stats)
    s = 0
    for k in stats:
        s += stats[k]
    print("s =", s)
    for k in stats:
        print(k, "\t", stats[k], "\t", str(round(stats[k] * 100.0 / s, 2)) + "%")
    print()
    f.close()
