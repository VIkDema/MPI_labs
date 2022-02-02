#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define SIZE 1000000
int main(int argc, char *argv[])
{
    char *string = argv[1];
    int N = atoi(string);
    double x[1000000];
    double TotalSum, ProcSum = 0.0;
    int ProcRank, ProcNum;
    MPI_Status Status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    if (ProcRank == 0)
    {
        for (int i = 0; i < N; i++)
            x[i] = rand() % (100);
    }
    double t = MPI_Wtime();
    double *rx = malloc(sizeof(double) * N / ProcNum);
    MPI_Scatter(x, N / ProcNum, MPI_DOUBLE,
                rx, N / ProcNum, MPI_DOUBLE,
                0, MPI_COMM_WORLD);
    for (int i = 0; i < N / ProcNum; ++i)
    {
        ProcSum += rx[i];
    }
    //printf("я %d процесс посчитал сумму %lf\n", ProcRank, ProcSum);
     (&ProcSum, &TotalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (ProcRank == 0)
    {
        FILE *S1;
        S1 = fopen("S1.txt", "a+");
        fprintf(S1, "%lf, ", MPI_Wtime() - t);
        //printf("%lf, ", MPI_Wtime() - t);
        fclose(S1);
    }
    free(rx);
    //printf("\nTotal Sum = %10.2f\n", TotalSum);
    MPI_Finalize();
}
