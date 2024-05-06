import subprocess

r = subprocess.run(["g++", "-std=c++20", "src/main.cpp"])
assert r.returncode == 0
r = subprocess.run(["./a.out"])
assert r.returncode == 0

import numpy as np
from pathlib import Path
import matplotlib.pyplot as plt

data = np.frombuffer(Path("img.bmp").read_bytes(),dtype=np.uint8).reshape(-1, 3)
data = data.reshape(int(data.shape[0]**0.5), int(data.shape[0]**0.5), 3)

plt.imsave("img.png", data)