#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define RANGE_START 2
#define RANGE_END   1000000
#define CHUNK_SIZE  10000
#define TAG_TASK    1
#define TAG_RESULT  2

static int is_prime(int n) {
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    int sq = (int)sqrt((double)n);
    for (int i = 3; i <= sq; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

static long count_primes(int start, int end) {
    long count = 0;
    for (int i = start; i <= end; i++)
        if (is_prime(i)) count++;
    return count;
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) fprintf(stderr, "Necessario ao menos 2 processos.\n");
        MPI_Finalize();
        return 1;
    }

    double t_start = MPI_Wtime();

    if (rank == 0) {
        int task[2];
        long result;
        MPI_Status status;
        long total_primes = 0;
        int next_start = RANGE_START;
        int active = 0;
        int *worker_chunks = calloc(size, sizeof(int));

        printf("Lider     : intervalo [%d, %d], chunk=%d, trabalhadores=%d\n",
               RANGE_START, RANGE_END, CHUNK_SIZE, size - 1);

        for (int w = 1; w < size; w++) {
            if (next_start <= RANGE_END) {
                task[0] = next_start;
                task[1] = (next_start + CHUNK_SIZE - 1 <= RANGE_END)
                          ? next_start + CHUNK_SIZE - 1 : RANGE_END;
                MPI_Send(task, 2, MPI_INT, w, TAG_TASK, MPI_COMM_WORLD);
                next_start = task[1] + 1;
                active++;
            } else {
                task[0] = -1; task[1] = -1;
                MPI_Send(task, 2, MPI_INT, w, TAG_TASK, MPI_COMM_WORLD);
            }
        }

        while (active > 0) {
            MPI_Recv(&result, 1, MPI_LONG, MPI_ANY_SOURCE, TAG_RESULT,
                     MPI_COMM_WORLD, &status);
            total_primes += result;
            int worker = status.MPI_SOURCE;
            worker_chunks[worker]++;

            if (next_start <= RANGE_END) {
                task[0] = next_start;
                task[1] = (next_start + CHUNK_SIZE - 1 <= RANGE_END)
                          ? next_start + CHUNK_SIZE - 1 : RANGE_END;
                MPI_Send(task, 2, MPI_INT, worker, TAG_TASK, MPI_COMM_WORLD);
                next_start = task[1] + 1;
            } else {
                task[0] = -1; task[1] = -1;
                MPI_Send(task, 2, MPI_INT, worker, TAG_TASK, MPI_COMM_WORLD);
                active--;
            }
        }

        double elapsed = MPI_Wtime() - t_start;

        printf("\nDistribuicao de chunks por trabalhador:\n");
        for (int w = 1; w < size; w++)
            printf("  Trabalhador %d: %d chunks processados\n", w, worker_chunks[w]);
        printf("\nPrimos em [%d, %d]: %ld\n", RANGE_START, RANGE_END, total_primes);
        printf("Tempo de execucao : %.6f s\n", elapsed);

        free(worker_chunks);

    } else {
        int task[2];
        long result;
        MPI_Status status;

        MPI_Recv(task, 2, MPI_INT, 0, TAG_TASK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        while (task[0] != -1) {
            result = count_primes(task[0], task[1]);
            MPI_Sendrecv(
                &result, 1, MPI_LONG, 0, TAG_RESULT,
                task,   2, MPI_INT,  0, TAG_TASK,
                MPI_COMM_WORLD, &status
            );
        }
    }

    MPI_Finalize();
    return 0;
}
