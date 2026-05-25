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
#define NX 2.0f // grid x length in meters(m)
#define NY 2.0f // grid y length in meters(m)
#define GRID_POINTS_X 100 // discrete points in x-axis 
#define GRID_POINTS_Y 100 // discrete points in y-axis
#define GRID_STEP_X (LX / (NX-1)) // (Δx) space between x-points
#define GRID_STEP_Y (LY / (NY-1)) // (Δy) space between y-points
float u[NX][NY]; // fluid velocity in x
float v[NX][NY]; // fluid velocity in y
int n = 0; // current time step index
float time_step = 0.0; // Δt time spacing

float cfl_compute_time_step(float dx, float dy, float viscosity){
  float time_step = ((dx*dx) * (dy*dy)) / 2*viscosity*((dx*dx) + (dy*dy)); 
  return time_step;
}


float update_velocity(float u, float v, int n, float dy, float dy, float dt, float viscosity){
  float u_new = 0.0;
  for(int i = 0; i < NX-2; i++){
    for(int j = 0; j < NY-2; j++){
      u_new = (u*n) + dt * (viscosity * (diffusion_in_x(u[i][j], u[i+1][j], u[i-1][j], dx) + diffusion_in_y(u[i][j], u[i][j+1], u[i][j-1], dy))) - advection_in_x(u[i][j], u[i+1][j], u[i-1][j], dx) - advection_in_y(v[i][j], u[i][j+1], u[i][j-1], dy));
    }
  }
  return u_new;
}

float diffusion_in_x(float u, float v, int n, float dx){
  float diffusion = ((u*n) - 2*(u*n) + u*n) / (dx*2);
  return diffusion;
}

float diffusion_in_y(float u, float v, int n, float dy){
  float diffusion = ((u*n) - 2*(u*n) + u*n) / (dy*2);
  return diffusion;
}

float advection_in_x(float u, float dx){
  float advection = (u*n)*((u*n) - u*n) / 2*dx;
  return advection;
}

float advection_in_y(float u, float v, float dy){
  float advection = (u*n)*((u*n) - u*n) / 2*dy;
  return advection;
}

int main(){

  return 0;
}

