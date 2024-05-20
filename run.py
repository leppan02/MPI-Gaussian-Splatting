import subprocess
# r = subprocess.run(["mpiCC", "-std=c++20", "-Wall","-fsanitize=address","-pedantic",  "src/main_mpi.cpp"])
# r = subprocess.run(["mpiCC", "-std=c++20", "-Wall","-pedantic",  "src/main_mpi.cpp"])
r = subprocess.run(["mpiCC", "-std=c++20", "-O3", "src/main_mpi.cpp"])
assert r.returncode == 0
from time import time
i = 4
r = subprocess.run(["mpirun","--oversubscribe", "-n", str(i), "./a.out"])
assert r.returncode == 0