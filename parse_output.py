from dataclasses import dataclass
from pathlib import Path
import re

re_match = r"""Data per process: (\d+)
Processes: (\d+)
Open file: (\d+)ms
Load positions: (\d+)ms
Sort positions: (\d+)ms
Load: (\d+)ms
Render: (\d+)ms
Communication: (\d+)ms"""

"""Data per process: 8797
Processes: 32
Open file: 432ms
Load positions: 0ms
Sort positions: 5ms
Load: 17ms
Render: 30ms
Communication: 173ms"""

@dataclass
class Data:
    data_per_process: int
    processes: int
    open_file: int
    load_positions: int
    sorting: int
    loading: int
    rendering: int
    communication: int

data: list[Data] = []

for p in Path('.').glob("slurm-394*.out"):
    data.extend(list(map(lambda x: Data(*map(int, x)), re.findall(re_match, p.read_text()))))

from matplotlib import pyplot as plt

from math import log

fig, ax = plt.subplots(3,1)


ax[0].loglog([d.processes for d in data], [d.loading for d in data], '.')
ax[0].loglog([d.processes for d in data], [d.rendering for d in data], '.')
ax[0].legend(['Loading', 'Rendering'])
ax[0].set_title('Loading and rendering time')


ax[1].plot([d.processes for d in data], [(d.loading)/d.data_per_process for d in data], '.')
ax[1].plot([d.processes for d in data], [(d.rendering)/d.data_per_process for d in data], '.')
ax[1].legend(['Loading', 'Rendering'])
ax[1].set_title('Loading and rendering time / amound of data')

ax[2].plot([d.processes for d in data], [ d.communication for d in data], '.')
ax[2].set_title('Communication time')

fig.legend()
plt.show()