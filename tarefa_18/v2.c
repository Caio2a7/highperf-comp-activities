/*
 * Tarefa 18 - Versao 2: distribuicao por colunas com MPI_Type_vector
 *             + MPI_Type_create_resized + MPI_Scatter
 *
 * O problema com usar MPI_Type_vector diretamente em MPI_Scatter e que
 * o extent do tipo e ((M-1)*N + local_cols)*sizeof(double) ~ tamanho
 * da matriz inteira. MPI_Scatter avanca esse extent por processo, o que
 * resulta em deslocamentos completamente errados para p > 0.
 *
 * MPI_Type_create_resized redefine o extent para local_cols*sizeof(double),
 * de forma que MPI_Scatter avance exatamente um bloco de colunas por processo:
 *   processo p acessa A a partir de A + p * local_cols (em doubles).
 *
 * Com o tipo redimensionado, MPI_Scatter(A, 1, col_block_resized, ...)
 * funciona corretamente: processo p recebe as colunas [p*local_cols,
 * (p+1)*local_cols - 1] de todas as M linhas, empacotadas em local_A.
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
        printf("Tarefa 18 v2: MPI_Type_vector + MPI_Type_create_resized\n");
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
         * Passo 1: tipo base que descreve um bloco de local_cols colunas.
         * count=M, blocklength=local_cols, stride=N (row-major).
         */
        MPI_Datatype col_block, col_block_resized;
        MPI_Type_vector(M, local_cols, N, MPI_DOUBLE, &col_block);

        /*
         * Passo 2: redefine o extent para local_cols*sizeof(double).
         * Sem isso, MPI_Scatter deslocaria ~M*N*sizeof(double) por processo
         * (o extent real do col_block), apontando para fora do buffer.
         * Com o extent corrigido, o processo p recebe a partir de
         * A[p * local_cols], que e o inicio de seu bloco de colunas.
         */
        MPI_Type_create_resized(col_block, 0,
                                (MPI_Aint)(local_cols * sizeof(double)),
                                &col_block_resized);
        MPI_Type_commit(&col_block_resized);
        MPI_Type_free(&col_block);

        MPI_Barrier(MPI_COMM_WORLD);
        double t_start = MPI_Wtime();

        /*
         * MPI_Scatter envia 1 elemento do tipo resized a cada processo.
         * O receptor usa MPI_DOUBLE porque local_A e contiguo em memoria:
         * as colunas sao desempacotadas linha a linha em local_A[i*local_cols+j].
         */
        MPI_Scatter(A, 1, col_block_resized,
                    local_A, M * local_cols, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);

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

        MPI_Type_free(&col_block_resized);
        free(local_A);
        free(local_x);
        free(local_y);
    }

    MPI_Finalize();
    return 0;
}
