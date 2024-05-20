from math import log
from matplotlib import pyplot as plt
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

for p in Path('.').glob("slurm-394196*.out"):
    data.extend(list(map(lambda x: Data(*map(int, x)),
                re.findall(re_match, p.read_text()))))


plt.plot([d.processes for d in data], [d.load_positions for d in data], '.')
plt.plot([d.processes for d in data], [d.sorting for d in data], '.')
plt.plot([d.processes for d in data], [d.loading for d in data], '.')
plt.plot([d.processes for d in data], [d.rendering for d in data], '.')
plt.plot([d.processes for d in data], [d.communication for d in data], '.')
plt.legend(["load_positions",
            "sorting",
            "loading",
            "rendering",
            "communication"])
plt.savefig("all.png")
plt.clf()

plt.loglog([d.processes for d in data], [d.load_positions for d in data], '.')
plt.loglog([d.processes for d in data], [d.sorting for d in data], '.')
plt.loglog([d.processes for d in data], [d.loading for d in data], '.')
plt.loglog([d.processes for d in data], [d.rendering for d in data], '.')
plt.loglog([d.processes for d in data], [d.communication for d in data], '.')
plt.legend(["load_positions",
            "sorting",
            "loading",
            "rendering",
            "communication"])
plt.savefig("all_loglog.png")
plt.clf()

plt.loglog([d.processes for d in data], [d.loading for d in data], '.')
plt.loglog([d.processes for d in data], [d.rendering for d in data], '.')
plt.legend(['Loading', 'Rendering'])
plt.title('Loading and rendering time')
plt.savefig("load_render.png")
plt.clf()

plt.plot([d.processes for d in data], [
         (d.loading)/d.data_per_process for d in data], '.')
plt.plot([d.processes for d in data], [
         (d.rendering)/d.data_per_process for d in data], '.')
plt.legend(['Loading', 'Rendering'])
plt.title('Loading and rendering time / amound of data')
plt.savefig("load_render_per_data.png")
plt.clf()

plt.plot([d.processes for d in data], [d.communication for d in data], '.')
plt.title('Communication time')

plt.savefig("comm.png")
plt.clf()
