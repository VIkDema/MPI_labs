/*
4 Поиск максимума в массиве. Процесс 0 генерирует массив и раздает его
другим процессам для поиска максимума. Собрав локальные максимумы,
ищет общий.
*/

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "mpi.h"
#define MAX_SIZE 1000000

void readData(int *size, int *arr)
{
    FILE *f = fopen("data.txt", "rt");
    int temp = 0;
    int count = 0;
    while (!feof(f))
    {
        fscanf(f, "%d", &temp);
        arr[count] = temp;
        count++;
        if (count == MAX_SIZE)
        {
            break;
        }
    }
    *size = count;
    fclose(f);
}

int searchMaxValue(int *arr, int size)
{
    int max = arr[0];
    for (size_t i = 1; i < size; i++)
    {
        if (arr[i] > max)
            max = arr[i];
    }
    return max;
}
int main(int argc, char *argv[])
{
    int ProcNum, ProcRank;
    int arr[MAX_SIZE];

    MPI_Status Status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    if (ProcRank == 0)
    {
        int size = 0;
        readData(&size, arr);
        for (int i = 0; i < size; i++)
        {
            //printf("%d, ", arr[i]);
        }
        //  printf("size: %d \n",size);

        if (ProcNum > 1)
        {
            double t1 = MPI_Wtime();

            int lenSubArrays = size / (ProcNum - 1);

            int lenLastSubArray = lenSubArrays + size % (ProcNum - 1);
            //отправка
            for (int i = 1; i < ProcNum; i++)
            {
                if (i != ProcNum - 1)
                {
                    MPI_Send(arr + lenSubArrays * (i - 1), lenSubArrays, MPI_INT, i, 0, MPI_COMM_WORLD);
                }
                else
                {
                    MPI_Send(arr + lenSubArrays * (i - 1), lenLastSubArray, MPI_INT, i, 0, MPI_COMM_WORLD);
                }
            }
            int result[ProcNum - 1];
            for (int i = 1; i < ProcNum; i++)
            {
                MPI_Recv(result + (i - 1), 1, MPI_INT, i, 0, MPI_COMM_WORLD, &Status);
            }
            int max = searchMaxValue(result, ProcNum - 1);
            double t2 = MPI_Wtime();
            if (ProcRank == 0)
            {
                printf("%lf,\n", t2 - t1);
            }
            //    printf("Max - %d\n", max);
        }
        else
        {
            double t1 = MPI_Wtime();
            int max = searchMaxValue(arr, size);
            double t2 = MPI_Wtime();
            if (ProcRank == 0)
            {
                printf("%lf,\n", t2 - t1);
            }
            //    printf("Max - %d\n", max);
        }
    }
    else
    {
        int size;
        MPI_Recv(&arr, MAX_SIZE, MPI_INT, 0, 0, MPI_COMM_WORLD, &Status);
        MPI_Get_count(&Status, MPI_INT, &size);
        // printf("%d - size: %d\n",ProcRank,size);
        int max = searchMaxValue(arr, size);
        //printf("local max process %d - %d\n",ProcRank,max);
        MPI_Send(&max, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}