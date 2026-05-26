#include <stdio.h>
#include <math.h>
#include <omp.h>

// u[i][j]n+1 = u[i][j]n
//            + Δt * 
//              { V * ( (u[i+1][j]n - 2*u[i][j]n + u[i-1][j]n) / Δx² + (u[i][j+1]n - 2*u[i][j]n + u[i][j-1]n) / Δy² )
//              - v[i][j]n * (u[i][j+1]n - u[i][j-1]n) / (2*Δy)
//              - u[i][j]n * (u[i+1][j]n - u[i-1][j]n) / (2*Δx) }

#define VISCOSITY 0.1f // kinematic viscosity constant (V) 
#define TIME 10.0f // simulation duration in seconds(s)
#define LX 10.0f // grid x length in meters(m)
#define LY 10.0f // grid y length in meters(m)
#define GRID_POINTS_X 10 // discrete points in x-axis 
#define GRID_POINTS_Y 10 // discrete points in y-axis
#define DX (LX / (GRID_POINTS_X-1)) // (Δx) space between x-points
#define DY (LY / (GRID_POINTS_Y-1)) // (Δy) space between y-points
float u[GRID_POINTS_X][GRID_POINTS_Y]; // fluid velocity in x
float v[GRID_POINTS_X][GRID_POINTS_Y]; // fluid velocity in y
int time_step = 0; // current time step index
float time_spacing = 1.0; // Δt time spacing

void print_grid(int nx, int ny, float grid[nx][ny]){
    printf("\n");
    for(int i = 2; i < nx-2; i++){
        printf("[ ");
        for(int j = 2; j < ny-2; j++){
            printf(" %0.5lf ", grid[i][j]);
        }
        printf(" ]\n");
    }
}

void initialize_grid(int nx, int ny){
    for(int i = 0; i < nx; i++){
        for(int j = 0; j < ny; j++){
            u[i][j] = 1.0f;
            v[i][j] = 1.0f;
        }
    }
}

void cfl_compute_time_spacing(float dx, float dy, float viscosity){
    time_spacing = ((dx*dx) * (dy*dy)) / (2*viscosity*((dx*dx) + (dy*dy)));
}

float diffusion_in_x(float first_term, float second_term, float third_term, float dx){
    return (first_term - 2*second_term + third_term) / (dx*dx);
}

float diffusion_in_y(float first_term, float second_term, float third_term, float dy){
    return (first_term - 2*second_term + third_term) / (dy*dy);
}

float advection_in_x(float first_term, float second_term, float third_term, float dx){
    return (first_term * (second_term - third_term)) / (dx*2);
}

float advection_in_y(float first_term, float second_term, float third_term, float dy){
    return (first_term * (second_term - third_term)) / (dy*2);
}

void update_velocity(float dx, float dy, float dt, int nx, int ny, float viscosity){
    float u_new[nx][ny];
    float v_new[nx][ny];
    
    for(int i = 1; i <= nx-2; i++){
        for(int j = 1; j <= ny-2; j++){
            u_new[i][j] = u[i][j] + dt * (viscosity * (diffusion_in_x(u[i+1][j], u[i][j], u[i-1][j], dx) + diffusion_in_y(u[i][j+1], u[i][j], u[i][j-1], dy)) - advection_in_y(v[i][j], u[i][j+1], u[i][j-1], dy) - advection_in_x(u[i][j], u[i+1][j], u[i-1][j], dx));
            v_new[i][j] = v[i][j] + dt * (viscosity * (diffusion_in_x(v[i+1][j], v[i][j], v[i-1][j], dx) + diffusion_in_y(v[i][j+1], v[i][j], v[i][j-1], dy)) - advection_in_y(v[i][j], v[i][j+1], v[i][j-1], dy) - advection_in_x(u[i][j], v[i+1][j], v[i-1][j], dx));
        }
    }
    
    for(int i = 1; i <= nx-2; i++){
        for(int j = 1; j <= ny-2; j++){
            u[i][j] = u_new[i][j];
            v[i][j] = v_new[i][j];
        }
    }
}

int main(){
    cfl_compute_time_spacing(DX, DY, VISCOSITY);
    initialize_grid(GRID_POINTS_X, GRID_POINTS_Y);
    u[GRID_POINTS_X/2][GRID_POINTS_Y/2] = 2.0f;
    v[GRID_POINTS_X/2][GRID_POINTS_Y/2] = 2.0f;
    float t = 0.0f;
    
    while(t < TIME){
        update_velocity(DX, DY, time_spacing, GRID_POINTS_X, GRID_POINTS_Y, VISCOSITY);
        print_grid(GRID_POINTS_X, GRID_POINTS_Y, u);
        t += time_spacing;
        time_step++;
    }
}
