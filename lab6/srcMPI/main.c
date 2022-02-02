
// Задание №1: Ленточный алгоритм 1 (горизонтальные полосы).
// n-высота, m-ширина
// умножение это строка на столбец
// Значит в каждом процесс есть первая строка,
// меняются строки другой матрицы,
// а к итоговой строке прибавляется одно умножение каждого элемента
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"
#define MAX_SIZE 500000

void bcast_sizes(int *nA, int *mA, int *nB, int *mB)
{
    int sizes[4];
    sizes[0] = *nA;
    sizes[1] = *mA;
    sizes[2] = *nB;
    sizes[3] = *mB;
    MPI_Bcast(sizes, 4, MPI_INT, 0, MPI_COMM_WORLD);
    *nA = sizes[0];
    *mA = sizes[1];
    *nB = sizes[2];
    *mB = sizes[3];
}

int *readMatrix(char *fileName, int *n, int *m)
{
    FILE *file = fopen(fileName, "r");
    if (file == NULL)
    {
        perror("Failed: ");
        return NULL;
    }
    fscanf(file, "%d %d\n", n, m);
    int *matrix = (int *)malloc(sizeof(int) * (*n) * (*m));
    for (int i = 0; i < *n; i++)
    {
        for (int j = 0; j < *m; j++)
        {
            int val = 0;
            fscanf(file, "%d ", &val);
            matrix[i * (*m) + j] = val;
        }
    }
    fclose(file);
    return matrix;
}

void writeMatrix(char *fileName, int *matrix, int n, int m)
{
    FILE *file = fopen(fileName, "w");
    if (file == NULL)
    {
        perror("Failed: ");
    }
    fprintf(file, "%d %d\n", n, m);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            fprintf(file, "%d ", matrix[m * i + j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

int main(int argc, char *argv[])
{
    char fileMatrixA[40] = "A.txt";
    char fileMatrixB[40] = "B.txt";
    char fileMatrixResult[40] = "result.txt";
    if (argc > 3)
    {
        strcpy(fileMatrixA, argv[1]);
        strcpy(fileMatrixB, argv[2]);
        strcpy(fileMatrixResult, argv[3]);
    }
    int ProcRank, ProcNum;
    int forTimer;
    MPI_Status Status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    int *A = NULL;
    int ARecv[MAX_SIZE];
    int *B = NULL;
    int BRecv[MAX_SIZE];
    int nA = 0, mA = 0, nB = 0, mB = 0;
    int *result = NULL;
    int resultSend[MAX_SIZE];
    double t;
    if (ProcRank == 0)
    {
        A = readMatrix(fileMatrixA, &nA, &mA);
        B = readMatrix(fileMatrixB, &nB, &mB);
        t = MPI_Wtime();
        if (ProcNum == 1)
        {
            int *matrix = (int *)malloc(sizeof(int) * nA * mB);
            for (size_t i = 0; i < nA; i++)
            {
                for (size_t j = 0; j < mB; j++)
                {
                    matrix[i * mB + j] = 0;
                    for (size_t k = 0; k < mA; k++)
                    {
                        matrix[i * mB + j] += A[i * mA + k] * B[k * mB + j];
                    }
                }
            }
            writeMatrix(fileMatrixResult, matrix, nA, mB);
            free(matrix);
        }
    }
    if (ProcNum > 1)
    {
        bcast_sizes(&nA, &mA, &nB, &mB); //каждый процесс знает размеры матриц.
        //получить строки матрицы A на каждом процессе
        int countRowsOnEachProc = nA / ProcNum; //на каждом
        int countRowsOnZeroProc = nA % ProcNum; //на zero
        int *sendCounts = (int *)malloc(sizeof(int) * ProcNum);
        int *displs = (int *)malloc(sizeof(int) * ProcNum);
        int *rowStartSecondMatrix = (int *)malloc(sizeof(int) * ProcNum);
        for (size_t i = 0; i < ProcNum; i++) //на нулевом есть дополнительные строки для отдельной обработки
        {
            if (i == 0)
            {
                rowStartSecondMatrix[i] = 0;
                displs[i] = 0;
                sendCounts[i] = (countRowsOnEachProc + countRowsOnZeroProc) * mA;
            }
            else
            {
                sendCounts[i] = countRowsOnEachProc * mA;
                displs[i] = (countRowsOnZeroProc + i * countRowsOnEachProc) * mA;
                rowStartSecondMatrix[i] = rowStartSecondMatrix[i - 1] + sendCounts[i - 1];
            }
        }
        MPI_Scatterv(A, sendCounts, displs, MPI_INT, ARecv, MAX_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatterv(B, sendCounts, displs, MPI_INT, BRecv, MAX_SIZE, MPI_INT, 0, MPI_COMM_WORLD);
        int count = sendCounts[ProcRank];
        int *res = (int *)malloc(sizeof(int) * sendCounts[ProcRank] * mB);
        for (size_t i = 0; i < sendCounts[ProcRank] * mB; i++)
        {
            res[i] = 0;
        }

        for (size_t i = 0; i < ProcNum; i++)
        {
            int nextProc = ProcRank + 1;
            if (nextProc == ProcNum)
            {
                nextProc = 0;
            }
            MPI_Send(BRecv, count, MPI_INT, nextProc, 0, MPI_COMM_WORLD);
            int prevProc = ProcRank - 1;
            if (prevProc < 0)
            {
                prevProc = ProcNum - 1;
            }
            MPI_Recv(BRecv, MAX_SIZE, MPI_INT, prevProc, 0,
                     MPI_COMM_WORLD, &Status);
            MPI_Get_count(&Status, MPI_INT, &count);
            for (size_t j = 0; j < count / mB; j++)
            {
                int startIndex = rowStartSecondMatrix[(ProcRank - 1 - (i) % ProcNum + ProcNum) % ProcNum] / mB;
                for (size_t l = 0; l < sendCounts[ProcRank] / mA; l++)
                {
                    int val = ARecv[startIndex + j + l * mA];
                    for (size_t k = 0; k < mB; k++)
                    {
                        res[l * mB + k] += val * BRecv[mB * j + k];
                    }
                }
            }
        }
        if (ProcRank == 0)
        {
            result = (int *)malloc(sizeof(int) * mA * nB);
        }
        MPI_Gatherv(res, sendCounts[ProcRank] / mA * mB, MPI_INT, result, sendCounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
        if (ProcRank == 0)
        {
            writeMatrix(fileMatrixResult, result, mA, nB);
            FILE *S1;
            S1 = fopen("time/time.txt", "a+");
            fprintf(S1, "%lf, ", MPI_Wtime() - t);
            fclose(S1);
        }
        free(sendCounts);
        free(displs);
        free(res);
    }
    else
    {
        FILE *S1;
        S1 = fopen("time/time.txt", "a+");
        fprintf(S1, "%lf, ", MPI_Wtime() - t);
        fclose(S1);
    }
    free(A);
    free(result);
    free(B);
    MPI_Finalize();
    return 0;
}
