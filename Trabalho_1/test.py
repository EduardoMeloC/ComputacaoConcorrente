import subprocess
import sys

with open("out.txt", "w+") as fout:
    times = []
    for i in range(5):
        out = subprocess.call(["./main.o", "0", "1"], stdout = fout);
        fout.seek(0)
        output = fout.read()
        times.append(int(output[-6:-2]))
    print(max(times))
