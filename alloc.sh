#!/bin/bash -l

# time allocation
#SBATCH -A edu24.sf2568
# 10 minutes wall-clock time will be given to this job
#SBATCH -t 00:60:00
# Number of tasks per core (prevent hyperthreading)
#SBATCH --ntasks-per-core=1
#SBATCH --cpus-per-task=1
# Use Dardel's main partition
#SBATCH -p main

# job name
# Number of nodes
#SBATCH --nodes=1
# Number of tasks per node
#SBATCH --ntasks-per-node=32
srun ./a.out