
/*
    Число процессов К кратно трем: K = 3N, N > 1 В процессах 0, N и 2N дано
    по N целых чисел. Определить для всех процессов декартову топологию 
    в виде матрицы размера 3 × N, после чего, используя функцию
    MPI_Cart_sub, расщепить матрицу процессов на три одномерные строки
    (при этом процессы 0, N и 2N будут главными процессами в полученных
    строках). Используя одну коллективную операцию пересылки данных,
    переслать по одному исходному числу из главного процесса каждой
    строки во все процессы этой же строки и вывести полученное число в
    каждом процессе (включая процессы 0, N и 2N).
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define SIZE 1000000

int main(int argc, char *argv[])
{
    srand(time(NULL));
    char *string = argv[1];
    int N = atoi(string);
    int ProcRank, ProcNum;
    int forTimer;
    MPI_Status Status;
    MPI_Init(&argc, &argv);
    double t = MPI_Wtime();
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    forTimer = ProcRank;
    MPI_Comm myComm;
    int dims[2];
    dims[0] = 3;
    dims[1] = ProcNum / 3;
    int periods[2];
    periods[0] = 0;
    periods[1] = 0;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &myComm);
    int coords[2];
    MPI_Cart_coords(myComm, ProcRank, 2, coords);
    // printf("%d %d\n", coords[0], coords[1]);

    MPI_Comm newComm;
    int remain_dims[] = {0, 1}; //первую кординату удаляем, остальные оставляем
    MPI_Cart_sub(myComm, remain_dims, &newComm);
    MPI_Comm_size(newComm, &ProcNum);
    MPI_Comm_rank(newComm, &ProcRank);
    //printf("%d rank\n", ProcRank);
    int *values = malloc(sizeof(int) * ProcNum * N);
    if (ProcRank == 0)
    {
        //    printf("Я процесс %d сгенерировал числа:", coords[0]);
        for (int i = 0; i < ProcNum * N; i++)
        {
            values[i] = rand() % ((coords[0] + coords[1] + 1) * (100));
            //          printf("%d ",values[i]);
        }
        //        printf("\n");
    }
    int *res = malloc(sizeof(int) * N);
    MPI_Scatter(values, N, MPI_INT, res, N, MPI_INT, 0, newComm);
    //printf("(%d  %d)", coords[0],coords[1]);
    for (int i = 0; i < N; i++)
    {
        //  printf("%d ",res[i]);
    }
    // printf("\n");

    if (forTimer == 0)
    {
        FILE *S1;
        S1 = fopen("S1.txt", "a+");
        fprintf(S1, "%lf, ", MPI_Wtime() - t);
        fclose(S1);
    }
    MPI_Finalize();
    return 0;
}
