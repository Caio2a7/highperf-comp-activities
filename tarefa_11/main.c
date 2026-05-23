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
#define LENGHT_X 2.0f // grid x length in meters(m)
#define LENGHT_Y 2.0f // grid y length in meters(m)
#define GRID_POINTS_X 100 // discrete points in x-axis 
#define GRID_POINTS_Y 100 // discrete points in y-axis
#define GRID_STEP_X (LX / (NX-1)) // (Δx) space between x-points
#define GRID_STEP_Y (LY / (NY-1)) // (Δy) space between y-points
float u; // fluid velocity in x
float v; // fluid velocity in y
int n; // current time step index
float time_step; // Δt time spacing

float cfl_compute_time_step(float dx, float dy, float viscosity){
  float time_step = ((dx*dx) * (dy*dy)) / 2*viscosity*((dx*dx) + (dy*dy)); 
  return time_step;
}


float update_velocity(){

}

float diffusion_in_x(float u, float v, int n, float dx){

}

float diffusion_in_y(float u, float v, int n, float dy){

}

float advection_in_x(){

}

float advection_in_y(){

}

int main(){

  return 0;
}

