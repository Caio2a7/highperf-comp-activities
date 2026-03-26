#include <stdio.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <time.h>

#define BENCHMARK_ITERATIONS 100000000

typedef struct timespec timespec;

double time_elapsed_ms(timespec start, timespec end){
    return (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1e6;
}

double pi_aprox_serie(int iterations){
    double pi = 0;
    #pragma omp parallel for reduction(+:pi)
    for(int i = 1; i < iterations; i++){
      pi += pow((sin(i) / i), 2.0);
    }
    return (pi * 2) + 1;
}


int main(){
    double pi = pi_aprox_serie(BENCHMARK_ITERATIONS );
    printf("\n PI: %lf \n", pi);
    return 0;
}
