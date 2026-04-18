#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#define CIRCLE_RADIUS 1
#define NUMBER_TRIES 100000000
_Bool is_in_radius(int radius, float x, float y){
    if (((x*x) + (y*y)) <= (radius*radius)){
        return 1;
    } 
    else{
        return 0;
    }
}
int pi_stochastic_approximation(int number_tries, int circle_radius){
    int numbers_in_range = 0;
    float x, y;
    int local_radius = circle_radius;
    #pragma omp parallel default(none) shared(numbers_in_range, number_tries, circle_radius, local_radius)
    {
        unsigned int seed = omp_get_thread_num();
        int local_count = 0;
        #pragma omp for private(x, y) firstprivate(local_radius)
        for(int i = 0; i < number_tries; i++){
            x = ((float)rand_r(&seed) / (float)RAND_MAX) * local_radius;
            y = ((float)rand_r(&seed) / (float)RAND_MAX) * local_radius;
            if(is_in_radius(local_radius, x, y) == 1){
                local_count++;
            }
        }
        #pragma omp critical
        numbers_in_range += local_count;
    }
    return numbers_in_range;
}
int main(){
    int numbers_in_range = pi_stochastic_approximation(NUMBER_TRIES, CIRCLE_RADIUS);
    double pi = ((double)numbers_in_range / (double)NUMBER_TRIES) * 4;
    printf("\nAproximação estocástica de pi: %.10lf\n", pi);
    return 0;
}
