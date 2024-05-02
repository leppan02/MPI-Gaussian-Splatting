import matplotlib.pyplot as plt
from pathlib import Path
import numpy as np

data = np.frombuffer(Path("img.bmp").read_bytes(),dtype=np.uint8).reshape(-1, 3)
data = data.reshape(int(data.shape[0]**0.5), int(data.shape[0]**0.5), 3)

plt.imsave("img.png", data)
