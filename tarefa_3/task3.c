#include <stdio.h>
#include <time.h>
#include <math.h>

typedef void(*gregory_liebniz)(int);
typedef struct timespec timespec;


double time_elapsed_ms(timespec start, timespec end){
  return (end.tv_sec - start.tv_sec) * 1000.00 + (end.tv_nsec - start.tv_nsec) / 1e6;
}

void gregory_liebniz_pi(int iterations){
  double pi = 0;
  for(int i = 1; i <= iterations; i++){
    pi += pow(-1, i - 1) / ((2.0 * i) - 1.0);
  }
  printf("\n * Executando para %d iterações * \n- Resultado: %.15lf\n", iterations, pi * 4.0);
}

void gregory_liebniz_pi_bechmark(gregory_liebniz fn, int iterations){
  timespec t_start, t_end;
  clock_gettime(CLOCK_MONOTONIC, &t_start);
  fn(iterations);
  clock_gettime(CLOCK_MONOTONIC, &t_end);
  printf("- Tempo executado: %.4f ms\n", time_elapsed_ms(t_start, t_end));
}

int main(){
  gregory_liebniz_pi_bechmark(gregory_liebniz_pi, 10);
  gregory_liebniz_pi_bechmark(gregory_liebniz_pi, 100);
  gregory_liebniz_pi_bechmark(gregory_liebniz_pi, 1000);
  gregory_liebniz_pi_bechmark(gregory_liebniz_pi, 10000);
  gregory_liebniz_pi_bechmark(gregory_liebniz_pi, 100000);
  gregory_liebniz_pi_bechmark(gregory_liebniz_pi, 1000000); // 1Mi
  gregory_liebniz_pi_bechmark(gregory_liebniz_pi, 10000000);
  gregory_liebniz_pi_bechmark(gregory_liebniz_pi, 100000000);
  gregory_liebniz_pi_bechmark(gregory_liebniz_pi, 1000000000); // 1Bi

  return 0;
}
