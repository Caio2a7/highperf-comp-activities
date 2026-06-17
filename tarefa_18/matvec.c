#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

static double run_v1(int M, int N, int rank, int size) {
    int local_cols = N / size;
    double *A = NULL, *y = NULL;
    double *x         = (double *)malloc(N              * sizeof(double));
    double *local_A   = (double *)malloc(M * local_cols * sizeof(double));
    double *local_x   = (double *)malloc(local_cols     * sizeof(double));
    double *partial_y = (double *)malloc(M              * sizeof(double));

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
    double t0 = MPI_Wtime();

    MPI_Datatype col_type;
    MPI_Type_vector(M, local_cols, N, MPI_DOUBLE, &col_type);
    MPI_Type_commit(&col_type);

    if (rank == 0) {
        for (int i = 0; i < M; i++)
            for (int j = 0; j < local_cols; j++)
                local_A[i * local_cols + j] = A[i * N + j];
        for (int p = 1; p < size; p++)
            MPI_Send(A + p * local_cols, 1, col_type, p, 0, MPI_COMM_WORLD);
    } else {
        MPI_Recv(local_A, M * local_cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Scatter(x, local_cols, MPI_DOUBLE, local_x, local_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < M; i++) {
        partial_y[i] = 0.0;
        for (int j = 0; j < local_cols; j++)
            partial_y[i] += local_A[i * local_cols + j] * local_x[j];
    }

    MPI_Reduce(partial_y, y, M, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double elapsed = MPI_Wtime() - t0;

    MPI_Type_free(&col_type);
    if (rank == 0) { free(A); free(y); }
    free(x); free(local_A); free(local_x); free(partial_y);
    return elapsed;
}

static double run_v2(int M, int N, int rank, int size) {
    int local_cols = N / size;
    double *A = NULL, *y = NULL;
    double *x         = (double *)malloc(N              * sizeof(double));
    double *local_A   = (double *)malloc(M * local_cols * sizeof(double));
    double *local_x   = (double *)malloc(local_cols     * sizeof(double));
    double *partial_y = (double *)malloc(M              * sizeof(double));

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
    double t0 = MPI_Wtime();

    MPI_Datatype col_type, col_resized;
    MPI_Type_vector(M, local_cols, N, MPI_DOUBLE, &col_type);
    MPI_Type_create_resized(col_type, 0, (MPI_Aint)(local_cols * sizeof(double)), &col_resized);
    MPI_Type_commit(&col_resized);

    MPI_Scatter(A, 1, col_resized, local_A, M * local_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(x, local_cols, MPI_DOUBLE, local_x, local_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < M; i++) {
        partial_y[i] = 0.0;
        for (int j = 0; j < local_cols; j++)
            partial_y[i] += local_A[i * local_cols + j] * local_x[j];
    }

    MPI_Reduce(partial_y, y, M, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double elapsed = MPI_Wtime() - t0;

    MPI_Type_free(&col_resized);
    MPI_Type_free(&col_type);
    if (rank == 0) { free(A); free(y); }
    free(x); free(local_A); free(local_x); free(partial_y);
    return elapsed;
}

int main(int argc, char *argv[]) {
    int rank, size;
    int cases[][2] = {{512, 512}, {1024, 1024}, {2048, 2048}, {4096, 4096}};
    int num_cases = 4;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Processos: %d\n", size);
        printf("%-15s | %-14s | %-12s\n", "Tamanho (MxN)", "V1-Type_vector", "V2-Resized");
        printf("%-15s-+-%-14s-+-%-12s\n", "---------------", "--------------", "------------");
    }

    for (int c = 0; c < num_cases; c++) {
        int M = cases[c][0], N = cases[c][1];
        if (N % size != 0) {
            if (rank == 0)
                printf("%-15s | SKIP (N %% p != 0)\n", "");
            continue;
        }
        double t1 = run_v1(M, N, rank, size);
        double t2 = run_v2(M, N, rank, size);
        if (rank == 0) {
            char label[32];
            snprintf(label, sizeof(label), "%dx%d", M, N);
            printf("%-15s | %-14.6f | %-12.6f\n", label, t1, t2);
        }
    }

    MPI_Finalize();
    return 0;
}
