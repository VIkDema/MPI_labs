#include <stdio.h>
#include "mpi.h"
#define SIZE 1000000

int checkMessage(char *message1, char *message2)
{
    unsigned int count = 0;
    for (int i = 0; i < SIZE; i++)
    {
        if (message1[i] != message2[i])
        {
            return 0;
        }
        count++;
    }
    return count;
}
void sendMessageAndCheckAnswer(char *message, char *answer, int ProcRank, char *from)
{
    double t1, t2, dt;
    MPI_Status Status;
    t1 = MPI_Wtime();
    MPI_Send(message, SIZE, MPI_CHAR, ProcRank + 1, 0, MPI_COMM_WORLD);
    MPI_Recv(answer, SIZE, MPI_INT, ProcRank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
    t2 = MPI_Wtime();
    dt = t2 - t1;
    int count = 0;
    if (count = checkMessage(message, answer))
    {
        printf("Я %s процесс с номером %d, мое сообщение из %d \"t\" отправилось верно, а на обмен мне потребовалось - %lf\n", from, ProcRank, count, dt);
        fflush (stdout);
    }
    else
    {
        printf("Я %s процесс с номером %d, мое сообщение отправилось неверно, а на обмен мне потребовалось - %lf\n", from, ProcRank, dt);
        fflush (stdout);
    }
}

int main(int argc, char *argv[])
{
    int ProcNum, ProcRank, RecvRank;
    MPI_Status Status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
    MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
    char message[SIZE];
    for (int i = 0; i < SIZE; ++i)
    {
        message[i] = 't';
    }
    char answer[SIZE];
    if (ProcRank == 0)
    {
        printf("Я четный процесс с номером %d отправляю сообщение процессу %d и жду ответ\n", ProcRank, ProcRank + 1);
        fflush (stdout);
        sendMessageAndCheckAnswer(message, answer, ProcRank, "четный");
    }
    else
    {
        if (ProcRank == ProcNum - 1)
        {
            MPI_Recv(answer, SIZE, MPI_INT, ProcRank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
            if (ProcRank % 2 == 0)
            {
                printf("Я последний четный процесс с номером %d, отправлю подтверждение, но никому дальше отправлять не буду\n", ProcRank);
                fflush (stdout);
                MPI_Send(message, SIZE, MPI_CHAR, ProcRank - 1, 0, MPI_COMM_WORLD);
            }
            else
            {
                printf("Я последний нечетный процесс с номером %d, отправлю подтверждение, никому дальше отправлять не буду\n", ProcRank);
                fflush (stdout);
                MPI_Send(message, SIZE, MPI_CHAR, ProcRank - 1, 0, MPI_COMM_WORLD);
            }
        }
        else if (ProcRank % 2 == 0)
        {
            MPI_Recv(answer, SIZE, MPI_INT, ProcRank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
            printf("Я четный процесс с номером %d получил сообщение от процесcа - %d, отправлю ему подтверждение\n", ProcRank, ProcRank - 1);
            fflush (stdout);
            MPI_Send(message, SIZE, MPI_CHAR, ProcRank - 1, 0, MPI_COMM_WORLD);
            sendMessageAndCheckAnswer(message, answer, ProcRank, "четный");
        }
        else
        {
            MPI_Recv(answer, SIZE, MPI_INT, ProcRank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &Status);
            printf("Я нечетный процесс с номером %d получил сообщение от процесcа - %d, отправлю ему подтверждение\n", ProcRank, ProcRank - 1);
            fflush (stdout);
            MPI_Send(message, SIZE, MPI_CHAR, ProcRank - 1, 0, MPI_COMM_WORLD);
            sendMessageAndCheckAnswer(message, answer, ProcRank, "нечетный");
        }
    }
    MPI_Finalize();
    return 0;
}