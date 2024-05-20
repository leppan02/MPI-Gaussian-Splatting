import numpy as np
from pathlib import Path
import matplotlib.pyplot as plt

for p in Path(".").glob("*.bmp"):
    data = np.frombuffer(p.read_bytes(),dtype=np.uint8).reshape(-1, 3)
    p.unlink()
    data = data.reshape(int(data.shape[0]**0.5), int(data.shape[0]**0.5), 3)

    plt.imsave(p.with_suffix('.png'), data)