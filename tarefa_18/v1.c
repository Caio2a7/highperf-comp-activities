/*
 * Tarefa 18 - Versao 1: distribuicao por colunas com MPI_Type_vector
 *
 * MPI_Type_vector descreve um bloco de colunas nao-contiguo em row-major:
 *   count       = M  (uma fatia por linha)
 *   blocklength = local_cols (colunas por processo)
 *   stride      = N  (passo entre fatias = largura da linha)
 *
 * O extent do tipo resultante e ((M-1)*N + local_cols)*sizeof(double), que e
 * aproximadamente o tamanho de toda a matriz. Por isso, MPI_Scatter nao pode
 * ser usado para enviar blocos de colunas consecutivos a processos distintos:
 * o deslocamento calculado por MPI_Scatter para o processo p seria
 * p * extent ~ p * M*N*sizeof(double), apontando para alem do buffer.
 * A solucao nesta versao e usar MPI_Send individual para cada processo,
 * com o tipo vetor para ler os dados nao-contiguos diretamente de A.
 *
 * Cada processo computa uma contribuicao parcial para y inteiro e
 * MPI_Reduce com MPI_SUM agrega as contribuicoes no processo 0.
 */

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
        printf("Tarefa 18 v1: MPI_Type_vector (distribuicao por colunas)\n");
        printf("Processos: %d\n", size);
        printf("%-15s | %-12s\n", "Tamanho (MxN)", "Tempo (s)");
        printf("%-15s-+-%-12s\n", "---------------", "------------");
    }

    for (int c = 0; c < num_cases; c++) {
        int M = cases[c][0];
        int N = cases[c][1];

        if (N % size != 0) {
            if (rank == 0)
                printf("%-15s | SKIP (N %% p != 0)\n", "");
            continue;
        }

        int local_cols = N / size;

        /* Rank 0 aloca a matriz completa; todos alocam buffers locais */
        double *A      = NULL;
        double *x_full = NULL;
        double *y      = NULL;
        double *local_A = (double *)malloc(M * local_cols * sizeof(double));
        double *local_x = (double *)malloc(local_cols       * sizeof(double));
        double *local_y = (double *)malloc(M                * sizeof(double));

        if (rank == 0) {
            A      = (double *)malloc(M * N * sizeof(double));
            x_full = (double *)malloc(N     * sizeof(double));
            y      = (double *)malloc(M     * sizeof(double));
            for (int i = 0; i < M; i++)
                for (int j = 0; j < N; j++)
                    A[i * N + j] = (double)(i + j + 1);
            for (int j = 0; j < N; j++)
                x_full[j] = (double)(j + 1);
        }

        /*
         * Tipo derivado: M blocos de local_cols doubles com stride N.
         * Aplicado a &A[p * local_cols], seleciona as local_cols colunas
         * do processo p em todas as M linhas da matriz row-major.
         */
        MPI_Datatype col_block;
        MPI_Type_vector(M, local_cols, N, MPI_DOUBLE, &col_block);
        MPI_Type_commit(&col_block);

        MPI_Barrier(MPI_COMM_WORLD);
        double t_start = MPI_Wtime();

        /*
         * Distribuicao das colunas via MPI_Send/Recv individuais.
         * O emissor usa col_block (acesso nao-contiguo em A);
         * o receptor recebe em buffer contiguo com MPI_DOUBLE.
         */
        if (rank == 0) {
            for (int p = 1; p < size; p++)
                MPI_Send(&A[p * local_cols], 1, col_block, p, 0, MPI_COMM_WORLD);
            /* Copia propria fatia de colunas para local_A */
            for (int i = 0; i < M; i++)
                for (int j = 0; j < local_cols; j++)
                    local_A[i * local_cols + j] = A[i * N + j];
        } else {
            MPI_Recv(local_A, M * local_cols, MPI_DOUBLE,
                     0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        /* Distribui o segmento de x correspondente as colunas de cada processo */
        MPI_Scatter(x_full, local_cols, MPI_DOUBLE,
                    local_x, local_cols, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);

        /* Contribuicao parcial: y_parcial[i] = sum_j(local_A[i,j] * local_x[j]) */
        for (int i = 0; i < M; i++) {
            local_y[i] = 0.0;
            for (int j = 0; j < local_cols; j++)
                local_y[i] += local_A[i * local_cols + j] * local_x[j];
        }

        /* Soma as contribuicoes parciais de todos os processos */
        MPI_Reduce(local_y, y, M, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);
        double t_total = MPI_Wtime() - t_start;

        if (rank == 0) {
            char label[32];
            snprintf(label, sizeof(label), "%dx%d", M, N);
            printf("%-15s | %.6f\n", label, t_total);
            free(A);
            free(x_full);
            free(y);
        }

        MPI_Type_free(&col_block);
        free(local_A);
        free(local_x);
        free(local_y);
    }

    MPI_Finalize();
    return 0;
}
