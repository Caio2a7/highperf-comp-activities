#include <stdio.h>
#include <math.h>
#include <omp.h>

#define DX 100000000
// intervalo [a,b]
double b = 10;
double a = 1;

typedef double (*target_function)(double);

void integral_aproximmation(target_function fn){
    double sum = 0;
    double dot = a;
    double h = (b - a) / (double)DX;
    for(int i = 1; i <= DX-1; i++){
        sum += fn(dot);
        dot = a+((h)*i);
    }
    double integral = h * ((fn(a)/2) + (fn(b)/2) + sum);
    printf("Integral: %.5lf\n", integral);
} 

void derivative_aproximmation(target_function fn){
    double h = (b - a) / (double)DX;
    
    double derivative = ((fn(a+h)) - (fn(a-h))) / (2*h);
    printf("Derivative: %.5lf\n", derivative);
}

double quadratic(double base){
    return pow(base, 2);
}

int main(){
    #pragma omp parallel sections
    {
        printf("TID: %d\n", omp_get_thread_num());
        #pragma omp section
        integral_aproximmation(quadratic);
        #pragma omp section
        derivative_aproximmation(quadratic);
    }    
    
    return 0;
}
