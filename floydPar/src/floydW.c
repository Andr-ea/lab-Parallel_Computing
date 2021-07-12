#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <stdbool.h>
#include <time.h>
#include <mpi.h>

#define ROOT    0

void floydWarshall(int *matrix, int N, int *matrix_proc, int *result);
void createMat(int *matrix_base, int N);
void printMat(int N, int *result);
int isPowerOfTwo(unsigned int x);
void saveMat(int N, int *result, char *path );
void readMat(char* file, int *matrix_base, int N);

int p_count, p_rank;
const int INF = 99999;

int main(int argc, char ** argv) {
    int N;
    int *matrix, *result, *matrix_proc;
    
    clock_t start, end;
    double cpu_time_used;

    if(argc != 2){
        printf("\n\nError ! -> Example: mpirun -np 2 ./test 4\n\n\n\n");
        return 1;
    }
    
    N = atoi(argv[1]);
    if(isPowerOfTwo(N) == false){
        printf("\n\nERROR ! The order of the matrix must be a power of 2\n\n\n\n");
        return 1;
    }
    
    MPI_Init(&argc, &argv); //-np val
    MPI_Comm_size(MPI_COMM_WORLD, &p_count);
    MPI_Comm_rank(MPI_COMM_WORLD, &p_rank);
    
    matrix_proc = (int*)malloc(sizeof(int) * N * (N / p_count)); //Matrix for each process
    
    //Root
    if (p_rank == ROOT){
        int matrix_size = sizeof(int)*N*N;
        matrix = (int*)malloc(matrix_size);
        
        //CREATE MATRIX
        //createMat(matrix, N);
        
        //READ MATRIX
        char path[40];
        snprintf(path, 40, "../data/matrix_generate_%dx%d.txt", N,N);
        readMat(path,matrix, N);
        
        // Allocate memory for the result matrix
        result = (int*)malloc(matrix_size);
    }
    
    //We make sure all processes are synchronized before starting
    MPI_Barrier(MPI_COMM_WORLD);
    
    if (p_rank == ROOT){
        // Get starting timepoint
        start = clock();
    }
    
    // FloydWarshall Algorithm
    floydWarshall(matrix, N, matrix_proc, result);
    
    if (p_rank == ROOT)
    {
        printf("----- FINISH ----- \n");
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        printf("Execution time  = %f sec\n", cpu_time_used);
        //Save the result matrix in a file "Result_Parallel_NxN.txt"
        char path[40];
        snprintf(path, 40, "../data/Result_Parallel_%dx%d.txt", N,N);
        saveMat(N, result, path);
    }
    
    MPI_Finalize();
    
}

void readMat(char* file,int* matrix_base, int N){
    FILE *fp;
    fp=fopen(file,"r");
    /*viene letta la matrice elemento per elemento*/
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            fscanf(fp,"%d",&matrix_base[i*N+j]);
        }
    }
    fclose(fp);
}

void createMat(int *matrix_base, int N){
  srand(time(NULL));
  int random_number;
  // Generate adjacency matrix randomly
  for(int i = 0; i < N*N; i ++) {
    random_number = (rand()%16);
    if (random_number == 0)
      matrix_base[i] = INF;
    else
      matrix_base[i] = random_number;
  }
   //Set the diagonal to 0
   for(int i = 0; i < N; i++)
      matrix_base[i*N + i] = 0;
    
    //Save the result matrix in a file "Result_Parallel_NxN.txt"
    char path[40];
    snprintf(path, 40, "../data/matrix_generate_%dx%d.txt", N,N);
   
    saveMat(N, matrix_base, path);
}

/* Print solution (Distance matrix) */
void printMat(int N, int *result) {
    printf("\n    ");
    printf ("The following matrix shows the shortest distances between every pair of vertices \n");
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            if (result[i*N+j] == INF)
                printf("%.1d ", INF);
            else
                printf ("%.1d ", result[i*N+j] );
        }
        printf("\n");
    }
}
/* Save matrix  */
void saveMat(int N, int *result, char *path) {
    FILE* f;
    f=fopen(path, "w");
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
                fprintf(f, "%.1d ", result[i*N+j]);
                
        }
        fprintf(f, "\n");
    }
    fclose(f);
}

//Check if x is a power of 2
int isPowerOfTwo(unsigned int x){
  while (((x % 2) == 0) && x > 1) // Finchè x pari e maggiore di 1
    x /= 2;
  return (x == 1);
}

void floydWarshall(int *matrix, int N, int *matrix_proc, int *result){
    
    int owner_of_k, offset_to_k, sum;
    int size_per_node = N / p_count;
    
    int row_size = sizeof(int) * N;
    int *row_k = (int*)malloc(row_size);
    int j, i, k;
    
    // Distributes data to all nodes from the ROOT processor
    MPI_Scatter(matrix, N * size_per_node, MPI_INT, matrix_proc, N * size_per_node, MPI_INT, 0, MPI_COMM_WORLD);
    
    for (k = 0; k < N; ++k){
        // We get the owner of the k that interests us
        owner_of_k = k / size_per_node;
        offset_to_k = (k % size_per_node) * N;
        // Transmit k row
        if (p_rank == owner_of_k)
            memcpy(row_k, matrix_proc + offset_to_k, row_size);
        MPI_Bcast(row_k, N, MPI_INT, owner_of_k, MPI_COMM_WORLD);
        
        for (i = 0; i < size_per_node; ++i)
        {
            for (j = 0; j < N; ++j)
            {
                if (row_k[j] != INF && matrix_proc[i*N + k] != INF)
                {
                    sum = matrix_proc[i*N + k] + row_k[j];
                    if (matrix_proc[i*N + j] > sum || matrix_proc[i*N + j] == INF) {
                        matrix_proc[i*N + j] = sum;
                    }
                }
            }
        }
    }
    // Gather the calculated values of all processes
    MPI_Gather(matrix_proc, N * size_per_node, MPI_INT, result, N * size_per_node, MPI_INT, 0, MPI_COMM_WORLD);
}
