#include <stdio.h>
#include <time.h>
#include <omp.h>

#define BENCHMARK_VECTOR_SIZE 100000000
#define BENCHMARK_VECTOR_INIT_VALUE 100000000

typedef struct timespec timespec;

int vector_a[BENCHMARK_VECTOR_SIZE];
int vector_b[BENCHMARK_VECTOR_SIZE];
int sum_vector[BENCHMARK_VECTOR_SIZE];

double time_elapsed_ms(timespec start, timespec end){
    return (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1e6;
}

void vec_init(int size, int vector[size]){
    for(int i = 0; i < size; i++){
        vector[i] = BENCHMARK_VECTOR_INIT_VALUE;
    }
}
void vec_print(int size, int vector[size]){
    printf("\n( ");
    for(int i = 0; i < size; i++){
        printf("%d ", vector[i]);
    }
    printf(" )\n");
}

void vec_sum(int size, int vector_a[size], int vector_b[size], int sum_vector[size]){
    // #pragma omp parallel for
    for(int i = 0; i < size; i++){
        sum_vector[i] = vector_a[i] + vector_b[i];
    }
}

int main(){
    vec_init(BENCHMARK_VECTOR_SIZE, vector_a);
    vec_init(BENCHMARK_VECTOR_SIZE, vector_b);
    vec_init(BENCHMARK_VECTOR_SIZE, sum_vector);
    vec_sum(BENCHMARK_VECTOR_SIZE, vector_a, vector_b, sum_vector);

    // vec_print(BENCHMARK_VECTOR_SIZE, sum_vector);
    return 0;
}

