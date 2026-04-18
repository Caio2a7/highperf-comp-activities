##include <stdio.h>
#include <omp.h>
#include <stdlib.h>

#define CIRCLE_RADIUS 1
#define NUMBER_TRIES 100000000

static inline int is_in_radius(int radius, float x, float y) {
    return ((x * x) + (y * y)) <= (float)(radius * radius);
}

long pi_v3(int tries, int radius) {
    long total = 0;
    float x, y;

    #pragma omp parallel default(none) shared(tries, radius, total) private(x, y)
    {
        unsigned int seed = (unsigned int)omp_get_thread_num();
        long local_count = 0;

        #pragma omp for
        for (int i = 0; i < tries; i++) {
            x = ((float)rand_r(&seed) / (float)RAND_MAX) * radius;
            y = ((float)rand_r(&seed) / (float)RAND_MAX) * radius;
            if (is_in_radius(radius, x, y))
                local_count++;
        }

        #pragma omp critical
        total += local_count;
    }
    return total;
}

int main(void) {
    double t0 = omp_get_wtime();
    long hits = pi_v3(NUMBER_TRIES, CIRCLE_RADIUS);
    double t1 = omp_get_wtime();

    double pi = 4.0 * (double)hits / NUMBER_TRIES;
    printf("Versão 3 [rand_r() + critical] pi ≈ %.10f  tempo = %.4f s\n", pi, t1 - t0);
    return 0;
}
