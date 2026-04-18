#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

#define CIRCLE_RADIUS 1
#define NUMBER_TRIES 100000000

static inline int is_in_radius(int radius, float x, float y) {
    return ((x * x) + (y * y)) <= (float)(radius * radius);
}

long pi_v4(int tries, int radius) {
    int nthreads;
    float x, y;

    #pragma omp parallel
    #pragma omp single
    nthreads = omp_get_num_threads();

    long *counts = calloc(nthreads, sizeof(long));

    #pragma omp parallel default(none) shared(tries, radius, counts, nthreads) private(x, y)
    {
        int tid = omp_get_thread_num();
        unsigned int seed = (unsigned int)tid;

        #pragma omp for
        for (int i = 0; i < tries; i++) {
            x = ((float)rand_r(&seed) / (float)RAND_MAX) * radius;
            y = ((float)rand_r(&seed) / (float)RAND_MAX) * radius;
            if (is_in_radius(radius, x, y))
                counts[tid]++;
        }
    }

    long total = 0;
    for (int t = 0; t < nthreads; t++)
        total += counts[t];

    free(counts);
    return total;
}

int main(void) {
    double t0 = omp_get_wtime();
    long hits = pi_v4(NUMBER_TRIES, CIRCLE_RADIUS);
    double t1 = omp_get_wtime();

    double pi = 4.0 * (double)hits / NUMBER_TRIES;
    printf("Versão 4 [rand_r() + vetor] pi ≈ %.10f  tempo = %.4f s\n", pi, t1 - t0);
    return 0;
}
