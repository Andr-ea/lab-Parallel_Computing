# lab-Parallel_Computing
Parallel implementation of Floyd-Warshall algorithm


- To compile and launch the parallel version, enter the floydPar src folder where the slurm file is located.
- The mpirun command, in the slurm file takes as input the size (power of 2) of the matrix created or to be created 
(es. mpirun -np 4 ./floydW.o 2048)
- In data folder there are matrices of sizes 128, 256, 512, 1024 and 2048

Create new matrix
- If you want to create a new matrix, uncomment line 53 of the floydPar.c class and comment lines 56,57
