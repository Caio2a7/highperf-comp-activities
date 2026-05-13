#include <stdio.h>
#include <math.h>
#include <omp.h>

#define DOTS_NUM 1000
#define dot_b 10
#define dot_a 1

typedef double (*target_function)(double);

void integral_aproximmation(target_function fn){
    double sum = 0;
    for(size_t i = 1; i <= DOTS_NUM-1; i++){
        sum += fn(i);
    }
    double dx = DOTS_NUM;
    double h = (dx - 1) / dx;
    double integral = h * ((fn(1)/2) + (fn(dx)/2) + sum);
    printf("Integral: %.5lf\n", integral);
} 

// void derivative_aproximmation(target_function fn, double x){

// }
double quadratic(double base){
    return pow(base, 2);
}

int main(){

    integral_aproximmation(quadratic);

    return 0;
}
