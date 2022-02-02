#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

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

void ratateArray(int *arr, int size)
{
    int last = arr[0];
    for (size_t i = 0; i < size - 1; i++)
    {
        arr[i] = arr[i + 1];
    }
    arr[size - 1] = last;
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
    if (argc > 4)
    {
        strcpy(fileMatrixA, argv[2]);
        strcpy(fileMatrixB, argv[3]);
        strcpy(fileMatrixResult, argv[4]);
    }
    omp_set_dynamic(0);
    omp_set_num_threads(atoi(argv[1]));
    clock_t start = clock();
    int *A = NULL;
    int *B = NULL;
    int *result = NULL;
    int N = 0;
    int *indexesB = NULL;
    int *countsB = NULL;
#pragma omp parallel shared(A, B, result, N, indexesB, countsB)
    {
        int i, n;
        i = omp_get_thread_num();
        n = omp_get_num_threads();
        if (i == 0)
        {
            A = readMatrix(fileMatrixA, &N, &N);
            B = readMatrix(fileMatrixB, &N, &N);
            indexesB = (int *)malloc(sizeof(int) * n);
            countsB = (int *)malloc(sizeof(int) * n);
            if (n == 1)
            {
                int *matrix = (int *)malloc(sizeof(int) * N * N);
                for (size_t t = 0; t < N; t++)
                {
                    for (size_t j = 0; j < N; j++)
                    {
                        matrix[t * N + j] = 0;
                        for (size_t k = 0; k < N; k++)
                        {
                            matrix[t * N + j] += A[t * N + k] * B[k * N + j];
                        }
                    }
                }
                writeMatrix(fileMatrixResult, matrix, N, N);
                free(matrix);
            }
        }
#pragma omp barrier
        if (n > 1)
        {
            if (i == 0)
            {
                result = (int *)malloc(sizeof(int) * N * N);
                for (size_t i = 0; i < N * N; i++)
                {
                    result[i] = 0;
                }
            }
#pragma omp barrier
            int countRowsOnEachThread = N / n;
            int countRowsOnZeroThread = N % n;
            int index = 0;
            int count = 0;
            if (i != 0)
            {
                index = countRowsOnZeroThread * N + countRowsOnEachThread * N * i;
                count = countRowsOnEachThread * N;
            }
            else
            {
                count = countRowsOnZeroThread * N + countRowsOnEachThread * N;
            }
            int *resRows = result + index;
            int *ARows = A + index;
            indexesB[i] = index;
            countsB[i] = count;
            for (size_t k = 0; k < n; k++)
            {
                for (size_t l = 0; l < count / N; l++)
                {
                    for (size_t m = 0; m < countsB[i] / N; m++)
                    {
                        int col = indexesB[i]/N + m;
                        int val = ARows[l * N + col];
                        for (size_t s = 0; s < N; s++)
                        {
                            resRows[l * N + s] += val * B[indexesB[i] + m * N + s];
                        }
                    }
                }
#pragma omp barrier
                if (i == 0)
                {
                    ratateArray(indexesB, n);
                    ratateArray(countsB, n);
                }
#pragma omp barrier
            }
            if (i == 0) {
                writeMatrix(fileMatrixResult, result, N, N);
            }
        }
    }
    clock_t stop = clock();
    double elapsed = (double)(stop - start) / CLOCKS_PER_SEC;
    FILE *S1;
    S1 = fopen("time/time2.txt", "a+");
    fprintf(S1, "%lf, ", elapsed);
    fclose(S1);
    free(indexesB);
    free(countsB);
    free(A);
    free(B);
    free(result);
    return 0;
}