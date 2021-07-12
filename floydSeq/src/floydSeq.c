#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>    // serve a dichiarare l'infinito, rappresentato nella costante UINT_MAX
#include <time.h>

typedef struct
{
    char name[100];
    int nnodes;
    int *content;        // metrice delle distanze iniziali
    int *dist;            // matrice delle distanze aggiornate
    int *pred;            // matrice che tiene traccia del predecessore di ogni nodo
} graph;


/* Definisco il valore di Infinito come un valore sufficientemente grande.
 Questo valore verrà utilizzato per i vertici non collegati tra loro */
#define INF 99999

int pos(int s, int i, int j);

void initialize_ist(graph *g, char *c, int nnodes);
// Funzione per stampare la matrice soluzione
void printSolution(graph *g);

// Risolve il problema del percorso più breve di tutte le coppie utilizzando l'algoritmo di Floyd Warshall
void floydWarshall (graph *g);

void printPred(graph *g);

void stampa(graph *g);

int isPowerOfTwo(unsigned int x);

void saveMat(graph *g, char *path);

/*
// ------------------ TRADUCE LA POSIZIONE DA MATRICE A VETTORE ------------------
*/
int pos(int s, int i, int j)
{
    int p = s*i+j;
    return p;
}

/* Generatore di numeri double casuali nel range [low, high] */
int random_int(int low,int high ){
    int k;
    int d;
    /* generation and normalization */
    d = (int) rand() / ((int) RAND_MAX);
    /* scale and truncation */
    k = (int) (d * (high - low + 1));
    /* translation */

    return(low + k);
}

/*
// ------------------ INIZIALIZZA LA STRUTTURA DEL GRAFO ------------------
*/
void initialize_ist(graph *g, char *c, int nnodes)
{
    FILE *fp;
    fp=fopen(c,"r");
    
    sprintf(g->name, "%s", c);
    g->nnodes = nnodes;
    //srand(time(NULL));    // genero un seme casuale in funzione del tempo
    g->content = (int *) calloc(nnodes*nnodes, sizeof(int));
    g->dist = (int *) calloc(nnodes*nnodes, sizeof(int));
    g->pred = (int *) calloc(nnodes*nnodes, sizeof(int));
    for ( int i = 0; i < nnodes; ++i)
    {
        for ( int j = 0; j < nnodes; ++j )
        {
            g->pred[pos(nnodes,i,j)] = i+1;             // inizilamente la matrice pred contiene in ogni elemento di ogni riga l'indice a quella riga
            
            //leggo da file invece che generare random, per generazione random guardare parte commentata sotto.
            fscanf(fp,"%d",&g->content[pos(nnodes,i,j)] );
            g->dist[pos(nnodes,i,j)] = g->content[pos(nnodes,i,j)];
           
           
        }
    }
    fclose(fp);
}

/* Stampa la soluzione (matrice delle distanze) */
void printSolution(graph *g)
{
    printf ("The following matrix shows the shortest distances between every pair of vertices in the problem of name: %s\n", g->name);
    for (int i = 0; i < g->nnodes; i++)
    {
        for (int j = 0; j < g->nnodes; j++)
        {
            if (g->dist[pos(g->nnodes,i,j)] == INF)
                printf("%.1s ", "INF");
            else
                printf ("%.1d ", g->dist[pos(g->nnodes,i,j)]);
        }
        printf("\n");
    }
}

void printPred(graph *g)
{
    printf ("The following matrix shows the predecessors matrix in the problem of name: %s\n", g->name);
    for (int i = 0; i < g->nnodes; i++)
        {
            for (int j = 0; j < g->nnodes; j++)
            {
                    printf ("%d ", g->pred[pos(g->nnodes,i,j)]);
            }
        printf("\n");
        }
}

//Controllo se x è potenza di 2
int isPowerOfTwo(unsigned int x){
  while (((x % 2) == 0) && x > 1) // Finchè x pari e maggiore di 1
    x /= 2;
  return (x == 1);
}

// Risolve il problema del percorso più breve di tutte le coppie utilizzando l'algoritmo di Floyd Warshall
void floydWarshall (graph *g)
{
    // dist[][] sarà la matrice di output che alla fine avrà le distanze più brevi tra ogni coppia di vertici

    // Aggiungi tutti i vertici uno per uno all'insieme dei vertici intermedi .
    // ---> Prima dell'inizio di un'iterazione, abbiamo le distanze più brevi tra tutte le coppie di vertici in modo tale che
    // le distanze più brevi considerino solo i vertici nell'insieme {0, 1, 2, .. k-1} come vertici intermedi.
    // ----> Dopo la fine di un'iterazione, il vertice k viene aggiunto all'insieme dei vertici intermedi e l'insieme diventa {0, 1, 2, .. k}
    for (int k = 0; k < g->nnodes; k++)
    {
        // Seleziona tutti i vertici come sorgente uno per uno
        for (int i = 0; i < g->nnodes; i++)
        {
            // Scegli tutti i vertici come destinazione per la sorgente selezionata sopra
            for (int j = 0; j < g->nnodes; j++)
            {
                // Se il vertice k è sul cammino più breve da i a j, aggiorna il valore di dist [i][j]
                if (g->dist[pos(g->nnodes,i,k)] + g->dist[pos(g->nnodes,k,j)] < g->dist[pos(g->nnodes,i,j)])
                {
                    g->pred[pos(g->nnodes,i,j)] = k+1;
                    g->dist[pos(g->nnodes,i,j)] = g->dist[pos(g->nnodes,i,k)] + g->dist[pos(g->nnodes,k,j)];
                }
            }
        }
    }
    
    // Stampa la matrice della distanza più breve/
    //printSolution(g);
    char path[40];
    snprintf(path, 40, "../data/Result_Sequential_%dx%d.txt", g->nnodes,g->nnodes);
    saveMat(g, path);
    //printPred(g);
}

/* Salva la matrice su file path */
void saveMat(graph *g, char *path) {
    FILE* f;
    f=fopen(path, "w");
    for (int i = 0; i < g->nnodes; i++){
        for (int j = 0; j < g->nnodes; j++){
                fprintf(f, "%.1d ", g->dist[pos(g->nnodes,i,j)] );
                
        }
        fprintf(f, "\n");
    }
    fclose(f);
}



void stampa(graph *g)
{
    printf ("The following matrix shows the initial matrix in the problem of name: %s\n", g->name);
    for(int i=0; i<g->nnodes; i++)
    {
        for(int j=0; j<g->nnodes; j++)
        {
            printf("%.1d ", g->content[pos(g->nnodes,i,j)]);
        }
        printf("\n");
    }
}

void free_instance( graph *g)
{
    free(g->content);
    free(g->dist);
    free(g->pred);
}

int main(int argc, char ** argv){
  
    int nnodes;
    graph g;
    
    clock_t start, end;
    double cpu_time_used;
    
    // Get starting timepoint
    start = clock();
    
    if(argc != 2){
        printf("\n\nError ! -> Example: \n\n\n\n");
        return 1;
    }
    
    nnodes = atoi(argv[1]);
    if(isPowerOfTwo(nnodes) == false){
        printf("\n\nERROR ! The order of the matrix must be a power of 2\n\n\n\n");
        return 1;
    }

    
    char file_name[40];
    snprintf(file_name, 40, "../data/matrix_generate_%dx%d.txt", nnodes,nnodes);
    initialize_ist(&g,file_name,nnodes);
    
    //lettura(file,nnodes);
    
    //stampa(&g);
    // Stampa soluzione
    
    floydWarshall(&g);
    
    printf("----- FINISH ----- \n");
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Execution time  = %f sec\n", cpu_time_used);
    
    free_instance(&g);
    return 0;
    

}

