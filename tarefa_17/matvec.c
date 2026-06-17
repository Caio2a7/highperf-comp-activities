#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int cases[][2] = {{512, 512}, {1024, 1024}, {2048, 2048}, {4096, 4096}};
    int num_cases = 4;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Processos: %d\n", size);
        printf("%-15s | %-12s\n", "Tamanho (MxN)", "Tempo (s)");
        printf("%-15s-+-%-12s\n", "---------------", "------------");
    }

    for (int c = 0; c < num_cases; c++) {
        int M = cases[c][0];
        int N = cases[c][1];

        if (M % size != 0) {
            if (rank == 0)
                printf("%-15s | SKIP (M %% p != 0)\n", "");
            continue;
        }

        int local_rows = M / size;

        double *A = NULL, *y = NULL;
        double *x       = (double *)malloc(N             * sizeof(double));
        double *local_A = (double *)malloc(local_rows * N * sizeof(double));
        double *local_y = (double *)malloc(local_rows     * sizeof(double));

        if (rank == 0) {
            A = (double *)malloc(M * N * sizeof(double));
            y = (double *)malloc(M     * sizeof(double));
            for (int i = 0; i < M; i++)
                for (int j = 0; j < N; j++)
                    A[i * N + j] = (double)(i + j + 1);
            for (int j = 0; j < N; j++)
                x[j] = (double)(j + 1);
        }

        MPI_Barrier(MPI_COMM_WORLD);
        double t_start = MPI_Wtime();

        MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Scatter(A, local_rows * N, MPI_DOUBLE,
                    local_A, local_rows * N, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);

        for (int i = 0; i < local_rows; i++) {
            local_y[i] = 0.0;
            for (int j = 0; j < N; j++)
                local_y[i] += local_A[i * N + j] * x[j];
        }

        MPI_Gather(local_y, local_rows, MPI_DOUBLE,
                   y, local_rows, MPI_DOUBLE,
                   0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
        double t_total = MPI_Wtime() - t_start;

        if (rank == 0) {
            char label[32];
            snprintf(label, sizeof(label), "%dx%d", M, N);
            printf("%-15s | %.6f\n", label, t_total);
            free(A);
            free(y);
        }

        free(x);
        free(local_A);
        free(local_y);
    }

    MPI_Finalize();
    return 0;
}
