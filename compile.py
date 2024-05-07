import subprocess

# r = subprocess.run(["g++", "-std=c++20", "src/main.cpp"])
# r = subprocess.run(["g++", "-std=c++20", "-Wall","-fsanitize=address" ,"-pedantic",  "src/main_mpi.cpp"])
r = subprocess.run(["g++", "-std=c++20", "-O3", "-Wall","-fsanitize=address" , "src/main_mpi.cpp"])
assert r.returncode == 0
r = subprocess.run(["./a.out"])
assert r.returncode == 0

import numpy as np
from pathlib import Path
import matplotlib.pyplot as plt

for p in Path(".").glob("*.bmp"):
    data = np.frombuffer(p.read_bytes(),dtype=np.uint8).reshape(-1, 3)
    p.unlink()
    data = data.reshape(int(data.shape[0]**0.5), int(data.shape[0]**0.5), 3)

    plt.imsave(p.with_suffix('.png'), data)