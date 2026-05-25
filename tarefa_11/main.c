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
#define LX 2.0f // grid x length in meters(m)
#define LY 2.0f // grid y length in meters(m)
#define GRID_POINTS_X 4 // discrete points in x-axis 
#define GRID_POINTS_Y 4 // discrete points in y-axis
#define DX (LX / (GRID_POINTS_X-1)) // (Δx) space between x-points
#define DY (LY / (GRID_POINTS_Y-1)) // (Δy) space between y-points
float u[GRID_POINTS_X][GRID_POINTS_Y]; // fluid velocity in x
float v[GRID_POINTS_X][GRID_POINTS_Y]; // fluid velocity in y
int time_step = 0; // current time step index
float time_spacing = 0.0; // Δt time spacing
float u_new = 0.0;

void initialize_grid(int nx, int ny){
    for(int i = 0; i < nx-2; i++){
        for(int j = 0; i < ny-2; j++){
            u[i][j] = 1.0f;
            v[i][j] = 1.0f;
            printf("U: %0.5lf - V: %0.5lf\n", u[i][j], v[i][j]);
        }
    }
}

float cfl_compute_time_spacing(float dx, float dy, float viscosity){
  return ((dx*dx) * (dy*dy)) / 2*viscosity*((dx*dx) + (dy*dy));
}

float diffusion_in_x(float first_term, float second_term, float third_term, int n, float dx){
  return ((powf(first_term, n)) - 2*(powf(second_term, n)) + powf(third_term, n)) / (dx*dx);
}

float diffusion_in_y(float first_term, float second_term, float third_term, int n, float dy){
  return ((powf(first_term, n)) - 2*(powf(second_term, n)) + powf(third_term, n)) / (dy*dy);
}

float advection_in_x(float first_term, float second_term, float third_term, int n, float dx){
  return ((powf(first_term, n)) * (powf(second_term, n) - powf(third_term, n))) / (dx*2);
}

float advection_in_y(float first_term, float second_term, float third_term, int n, float dy){
  return ((powf(first_term, n)) * (powf(second_term, n) - powf(third_term, n))) / (dy*2);
}

void update_velocity(int n, float dx, float dy, float dt, int nx, int ny, float viscosity){
  for(int i = 0; i < nx-2; i++){
    for(int j = 0; j < ny-2; j++){
      u_new = (powf(u[i][j], n)) + dt * (viscosity * (diffusion_in_x(u[i+1][j], u[i][j], u[i-1][j], n, dx) + diffusion_in_y(u[i][j+1], u[i][j], u[i][j-1], n, dy)) - advection_in_y(v[i][j], u[i][j+1], u[i][j-1], n, dy) - advection_in_x(u[i][j], u[i+1][j], u[i-1][j], dx, n));
      printf("U NEW: %0.5lf", u_new);
    }
  }
}

int main(){
    time_spacing = cfl_compute_time_spacing(DX, DY, VISCOSITY);
    printf("TIME SPACING: %0.5lf\n", time_spacing);
    initialize_grid(GRID_POINTS_X, GRID_POINTS_Y);
    update_velocity(time_step, DX, DY, time_spacing, GRID_POINTS_X, GRID_POINTS_Y, VISCOSITY);
    printf("U velocity: %0.10lf\n", u_new);
}
