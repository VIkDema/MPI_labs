
/*
В каждом процессе, ранг которого делится на 3 (включая главный процесс),
даны три целых числа. С помощью функции MPI_Comm_split создать
новый коммуникатор, включающий процессы, ранг которых делится на 3
Используя одну коллективную операцию пересылки данных для
созданного коммуникатора, переслать исходные числа в главный
процесс и вывести эти числа в порядке возрастания рангов переславших
их процессов (включая числа, полученные из главного процесса).
Указание. При вызове функции MPI_Comm_split в процессах, которые не
требуется включать в новый коммуникатор, в качестве параметра color
следует указывать константу MPI_UNDEFINED.
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
    MPI_Status Status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    double t = MPI_Wtime();
    MPI_Comm myComm;
    int color = 0;
    if (ProcRank % 3 != 0)
    {
        color = MPI_UNDEFINED;
    }
    //параметр key указывает на порядок, для сохранения порядка key=ProcRank.
    MPI_Comm_split(MPI_COMM_WORLD, color, ProcRank, &myComm);
    if (myComm != MPI_COMM_NULL)
    {
        int* values=malloc(sizeof(int)*(N));
        //printf("я процесс %d сгенерировал:",ProcRank);
        for (int i = 0; i < N; ++i)
        {
            values[i] = rand() % ((ProcRank + 1) * 100);
          //  printf("%d, ",values[i]);
        }
       // printf("\n");
        int myCommSize = 0;
        MPI_Comm_size(myComm, &myCommSize);
        int *res = malloc(sizeof(int) * N * myCommSize);
        MPI_Gather(values, N, MPI_INT, res, N, MPI_INT, 0, myComm);
        if (ProcRank == 0)
        {
            for (int i = 0; i < N * myCommSize; ++i)
            {
               // printf("%d ", res[i]);
            }
         //   printf("\n");
        }
    }
    if (ProcRank == 0)
    {
        FILE *S1;
        S1 = fopen("S1.txt", "a+");
        fprintf(S1, "%lf, ", MPI_Wtime() - t);
        fclose(S1);
    }
    MPI_Finalize();
    return 0;
}
