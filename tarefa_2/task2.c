#include <stdio.h>
#include <time.h>

#define VECTOR_SIZE 800000

int vector[VECTOR_SIZE];

double time_elapsed_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 +
           (end.tv_nsec - start.tv_nsec) / 1e6;
}

void vector_init(int size, int vector[size]) {
    for (int i = 0; i < size; i++)
        vector[i] = i * 2 + 1;
}

int cumulative_sum(int size, int vector[size]) {
    int sum = 0;
    for (int i = 0; i < size; i++)
        sum += vector[i];
    return sum;
}

int multiple_vars_sum(int size, int vector[size]) {
    int s0 = 0, s1 = 0, s2 = 0, s3 = 0,
        s4 = 0, s5 = 0, s6 = 0, s7 = 0;
    for (int i = 0; i < size; i += 8) {
        s0 += vector[i];
        s1 += vector[i + 1];
        s2 += vector[i + 2];
        s3 += vector[i + 3];
        s4 += vector[i + 4];
        s5 += vector[i + 5];
        s6 += vector[i + 6];
        s7 += vector[i + 7];
    }
    return s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7;
}

int main(void) {
    struct timespec t_start, t_end;

    // loop 1: inicialização
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    vector_init(VECTOR_SIZE, vector);
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    printf("%-30s time: %.6f ms\n", "init", time_elapsed_ms(t_start, t_end));

    // loop 2: soma acumulativa (dependência)
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    int r1 = cumulative_sum(VECTOR_SIZE, vector);
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    printf("%-30s sum: %-12d time: %.6f ms\n", "cumulative", r1, time_elapsed_ms(t_start, t_end));

    // loop 3: múltiplas variáveis (sem dependência)
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    int r2 = multiple_vars_sum(VECTOR_SIZE, vector);
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    printf("%-30s sum: %-12d time: %.6f ms\n", "multiple 8 vars", r2, time_elapsed_ms(t_start, t_end));

    return 0;
}
