#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define CIRCLE_RADIUS 1
#define NUMBER_TRIES 1000000

_Bool is_in_radius(int radius, float x, float y){
    if (((x*x) + (y*y)) <= (radius*radius)){
        return 1;
    } 
    else{
        return 0;
    }
}

int pi_stochastic_approximation(int number_tries, int circle_radius){
    int private_numbers_in_range = 0, public_numbers_in_range = 0;
    #pragma omp parallel num_threads(2) firstprivate(private_numbers_in_range)
    {
        #pragma omp for
        for(int i = 0; i < number_tries; i++){
            float x = ((float)rand() / (float)RAND_MAX) * circle_radius;
            float y = ((float)rand() / (float)RAND_MAX) * circle_radius;
            if(is_in_radius(circle_radius, x, y) == 1){
                private_numbers_in_range++;
            }
        }
        printf("TID: %d - Numbers in Range: %d\n", omp_get_thread_num(), private_numbers_in_range);
        #pragma omp critical
        public_numbers_in_range += private_numbers_in_range;
    }
    return public_numbers_in_range;
}

int main(){
    srand(time(NULL));
    int numbers_in_range = pi_stochastic_approximation(NUMBER_TRIES, CIRCLE_RADIUS);
    double pi = ((double)numbers_in_range / (double)NUMBER_TRIES) * 4;
    printf("\nAproximação estocástica de pi: %.10lf\n", pi);
    return 0;
}
