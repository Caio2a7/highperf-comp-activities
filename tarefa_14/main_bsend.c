#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ITERACOES 1000

int main(int argc, char *argv[]) {
    int rank;
    double inicio, fim;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int tamanhos[] = {8, 1024, 65536, 524288, 1048576};

    for (int s = 0; s < 5; s++) {
        int n = tamanhos[s];
        char *buf = malloc(n);
        memset(buf, 'A', n);

        int bufsize = n + MPI_BSEND_OVERHEAD;
        char *bsend_buf = malloc(bufsize);
        MPI_Buffer_attach(bsend_buf, bufsize);

        MPI_Barrier(MPI_COMM_WORLD);

        if (rank == 0) {
            inicio = MPI_Wtime();
            for (int i = 0; i < ITERACOES; i++) {
                MPI_Bsend(buf, n, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(buf, n, MPI_CHAR, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            fim = MPI_Wtime();
            printf("Tamanho: %7d bytes | Tempo total: %.6f s | Media: %.3f us\n",
                   n, fim - inicio, ((fim - inicio) / ITERACOES) * 1e6);
        } else {
            for (int i = 0; i < ITERACOES; i++) {
                MPI_Recv(buf, n, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Bsend(buf, n, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }

        void *returned_buf;
        int returned_size;
        MPI_Buffer_detach(&returned_buf, &returned_size);
        free(returned_buf);
        free(buf);
    }

    MPI_Finalize();
    return 0;
}
