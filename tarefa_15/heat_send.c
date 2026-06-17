#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define N      1000
#define STEPS  10000
#define R      0.25

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int local_n = N / size + (rank == size - 1 ? N % size : 0);
    int global_start = rank * (N / size);

    double *u     = calloc(local_n + 2, sizeof(double));
    double *u_new = calloc(local_n + 2, sizeof(double));

    for (int i = 1; i <= local_n; i++)
        if (global_start + (i - 1) == N / 2)
            u[i] = 100.0;

    if (rank == 0) {
        printf("Versao   : MPI_Send / MPI_Recv\n");
        printf("Processos: %d | Celulas: %d | Passos: %d | r: %.2f\n", size, N, STEPS, R);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    printf("Processo %d -> celulas globais [%d, %d]\n", rank, global_start, global_start + local_n - 1);
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    double t_start = MPI_Wtime();

    for (int step = 0; step < STEPS; step++) {
        if (rank < size - 1)
            MPI_Send(&u[local_n], 1, MPI_DOUBLE, rank + 1, 0, MPI_COMM_WORLD);
        if (rank > 0)
            MPI_Recv(&u[0], 1, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (rank > 0)
            MPI_Send(&u[1], 1, MPI_DOUBLE, rank - 1, 1, MPI_COMM_WORLD);
        if (rank < size - 1)
            MPI_Recv(&u[local_n + 1], 1, MPI_DOUBLE, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = 1; i <= local_n; i++)
            u_new[i] = u[i] + R * (u[i - 1] - 2.0 * u[i] + u[i + 1]);

        double *tmp = u; u = u_new; u_new = tmp;
    }

    double elapsed = MPI_Wtime() - t_start;
    double max_elapsed;
    MPI_Reduce(&elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    double local_max = 0.0;
    for (int i = 1; i <= local_n; i++)
        if (u[i] > local_max) local_max = u[i];

    MPI_Barrier(MPI_COMM_WORLD);
    printf("Processo %d -> temp. maxima local apos %d passos: %.6f\n", rank, STEPS, local_max);
    fflush(stdout);
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0)
        printf("\nTempo de execucao: %.6f s\n", max_elapsed);

    free(u);
    free(u_new);
    MPI_Finalize();
    return 0;
}
