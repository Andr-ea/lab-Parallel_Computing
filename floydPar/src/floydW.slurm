#!/bin/bash

#SBATCH --job-name fwparallel
#SBATCH --error fwparallel_err_O1_4_2048_%j.txt
#SBATCH --output fwparallel_out_O1_4_2048_%j.txt
#SBATCH --partition allgroups
#SBATCH --ntasks 4
#SBATCH -c 1
#SBATCH --mem 4G
#SBATCH --time 10:00

#spack load openmpi@3.1.4
spack load intel-parallel-studio@professional.2019.4

#mpicc -O1 floydW.c -o floydW.o
mpirun -np 4 ./floydW.o 2048
