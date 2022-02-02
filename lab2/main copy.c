/*
5 Игра в снежки. Процессы делятся на 2 группы (четные и нечетные
номера). Каждый процесс нечетной группы случайным образом генерирует
посылку четному процессу. Четный процесс, получив посылку, в свою 
очередь, отправляет случайно выбранному нечетному процессу следующую.
Предусмотреть ситуацию получения одним процессом нескольких посылок.
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
int generateRandomValue(int range, int fromProcRank)
{
    int res = fromProcRank;
    while (res == fromProcRank || fromProcRank % 2 == 0 && res % 2 == 0 || fromProcRank % 2 == 1 && res % 2 == 1)
    {
        res = rand() % (range);
    }
    printf("%d Кидаю снежок процессу %d\n", fromProcRank, res);
    return res;
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    int ProcNum, ProcRank;
    int count_of_snowballs = 5;
    char MySnowBall[] = "snowball    ";
    char fromSnowball[12] = "\0";
    int flag = -1;
    int gettingMessage = 0;

    MPI_Request request;
    MPI_Status Status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    sprintf(MySnowBall + 9, "%d", ProcRank);
    double t = MPI_Wtime();

    if (ProcRank % 2 == 1)
    {
        MPI_Send(MySnowBall, 12, MPI_CHAR, generateRandomValue(ProcNum, ProcRank), 0, MPI_COMM_WORLD);
    }
    double rt = MPI_Wtime();
    while (MPI_Wtime() - t < 5)
    {
        for (int i = 0; i < 1000; ++i)
        {
            if (flag != 0)
            {
                MPI_Irecv(fromSnowball, 12, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
                flag = 0;
                i = 0;
            }
            MPI_Test(&request, &flag, &Status);
            if (flag != 0)
            {
                i = 0;
                gettingMessage = 1;
            }
        }
        if (gettingMessage == 1)
        {
            printf("%d в меня кто-то попал \n", ProcRank);
            if (count_of_snowballs == 0)
            {
                printf("%d Закончились снежки \n", ProcRank);
            }
            else
            {
                printf("%d осталось снежков %d\n", ProcRank, count_of_snowballs);
                MPI_Send(MySnowBall, 12, MPI_CHAR, generateRandomValue(ProcNum, ProcRank), 0, MPI_COMM_WORLD);
                count_of_snowballs--;
            }
            gettingMessage = 0;
        }
    }
    if (ProcRank == 0)
    {
        FILE *S1;
        S1 = fopen("S1.txt", "w");
        fprintf(S1, "%lf, ", MPI_Wtime() - rt);
        printf("%lf, ", MPI_Wtime() - rt);
        fclose(S1);
    }
    printf("%d Выхожу из игры, осталось снежков %d\n", ProcRank, count_of_snowballs);
    MPI_Finalize();
    return 0;
}