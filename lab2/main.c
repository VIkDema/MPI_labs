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
    int N = 12;
    srand(time(NULL));
    int ProcNum, ProcRank;
    int count_of_snowballs = 3;
    char MySnowBall[12];
    char fromSnowball[15][12] = {"\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0", "\0"};
    int flag = -1;
    int gettingMessage = 0;

    MPI_Request r_request;
    MPI_Request s_request;
    MPI_Status Status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    sprintf(MySnowBall+9, "%d", ProcRank);
    double rt = MPI_Wtime();
    if (ProcRank % 2 == 1)
    {
        MPI_Send(MySnowBall, N, MPI_CHAR, generateRandomValue(ProcNum, ProcRank), 0, MPI_COMM_WORLD);
        count_of_snowballs--;
        printf("%d осталось снежков %d\n", ProcRank, count_of_snowballs);
    }

    int flags[15] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
    MPI_Request requests[15];
    MPI_Status Statuss[15];
    //получение четным процессом
    for (int i = 0; i < 1000000; ++i)
    {
        for (int i = 0; i < ProcNum; ++i)
        {
            if (ProcRank % 2 == 0 && i % 2 == 1 || ProcRank % 2 == 1 && i % 2 == 0)
            {
                if (flags[i])
                    MPI_Irecv(fromSnowball[i], N, MPI_CHAR, i, MPI_ANY_TAG, MPI_COMM_WORLD, &requests[i]);
                MPI_Test(&requests[i], &flags[i], &Statuss[i]);
                //printf("%d проверю летит ли снежок от процесса %d\n", ProcRank, i);
            }
        }
        int count = 0;
        for (int i = 0; i < ProcNum; ++i)
        {

            if (ProcRank % 2 == 0 && i % 2 == 1 || ProcRank % 2 == 1 && i % 2 == 0)
                if (flags[i])
                {
                    printf("%d Получен снежок от процесса %d\n", ProcRank, i);
                    count++;
                }
        }
        if (count)
        {
            printf("%d Получено %d снежков\n", ProcRank, count);
            if (count_of_snowballs > 0)
            {
                MPI_Send(MySnowBall, N, MPI_CHAR, generateRandomValue(ProcNum, ProcRank), 0, MPI_COMM_WORLD);
                count_of_snowballs--;
                i=0;
            }
            else
            {
               printf("%d снежки закончились\n", ProcRank);
            }
        }
    }

    if (ProcRank == 0)
    {
        FILE *S1;
        S1 = fopen("S1.txt", "a+");
        fprintf(S1, "%lf, ", MPI_Wtime() - rt);
        fclose(S1);
    }
    printf("%d Выхожу из игры, осталось снежков %d\n", ProcRank, count_of_snowballs);
    MPI_Finalize();
    return 0;
}