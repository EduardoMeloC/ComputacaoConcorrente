import subprocess
import statistics
import sys

with open("out.txt", "w+") as fout:
    for nthreads in range(5):
        for nsamples in [1, 4, 16, 64]:
            times = []
            for i in range(5):
                out = subprocess.call(["./main.o", str(nthreads), str(nsamples)], stdout = fout);
                fout.seek(0)
                output = fout.read()
                times.append(float(output[output.rindex("\t")+1:-2]))
                print(times)
            print("nthreads = ", nthreads, "nsamples = ", nsamples)
            print(statistics.mean(times), "\n")
