
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include "raylib.h"

#define VISCOSITY 0.1f
#define TIME 10.0f
#define LX 20.0f
#define LY 20.0f
#define GRID_POINTS_X 200
#define GRID_POINTS_Y 200
#define DX (LX / (GRID_POINTS_X-1))
#define DY (LY / (GRID_POINTS_Y-1))
#define WINDOW_W 800
#define WINDOW_H 800
#define CELL_W (WINDOW_W / GRID_POINTS_X)
#define CELL_H (WINDOW_H / GRID_POINTS_Y)

float u[GRID_POINTS_X][GRID_POINTS_Y];
float v[GRID_POINTS_X][GRID_POINTS_Y];
int time_step = 0;
float time_spacing = 1.0;

void render_grid(int nx, int ny, float grid[nx][ny]) {
    float min_val = grid[0][0], max_val = grid[0][0];
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++) {
            if (grid[i][j] < min_val) min_val = grid[i][j];
            if (grid[i][j] > max_val) max_val = grid[i][j];
        }
    float range = max_val - min_val;
    if (range == 0.0f) range = 1.0f;

    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            float t = (grid[i][j] - min_val) / range;
            Color c = ColorFromHSV((1.0f - t) * 240.0f, 1.0f, 1.0f);

            int x0 = (i * WINDOW_W) / nx;
            int x1 = ((i + 1) * WINDOW_W) / nx;
            int y0 = (j * WINDOW_H) / ny;
            int y1 = ((j + 1) * WINDOW_H) / ny;

            DrawRectangle(x0, y0, x1 - x0, y1 - y0, c);
        }
    }
}

void initialize_grid(int nx, int ny) {
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++) {
            u[i][j] = 1.0f;
            v[i][j] = 1.0f;
        }
}

void initialize_perturbation(int nx, int ny) {
    float cx = LX / 2.0f;
    float cy = LY / 2.0f;
    float radius = LX * 0.1f;

    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            float x = i * DX;
            float y = j * DY;
            float dist = sqrtf((x - cx)*(x - cx) + (y - cy)*(y - cy));
            if (dist < radius) {
                u[i][j] += 1.0f;
            }
        }
    }
}

void cfl_compute_time_spacing(float dx, float dy, float viscosity) {
    time_spacing = ((dx*dx) * (dy*dy)) / (2*viscosity*((dx*dx) + (dy*dy)));
}

float diffusion_in_x(float first_term, float second_term, float third_term, float dx) {
    return (first_term - 2*second_term + third_term) / (dx*dx);
}

float diffusion_in_y(float first_term, float second_term, float third_term, float dy) {
    return (first_term - 2*second_term + third_term) / (dy*dy);
}

float advection_in_x(float first_term, float second_term, float third_term, float dx) {
    return (first_term * (second_term - third_term)) / (dx*2);
}

float advection_in_y(float first_term, float second_term, float third_term, float dy) {
    return (first_term * (second_term - third_term)) / (dy*2);
}

void update_velocity(float dx, float dy, float dt, int nx, int ny, float viscosity) {
    float u_new[nx][ny];
    float v_new[nx][ny];

    for (int i = 1; i <= nx-2; i++) {
        for (int j = 1; j <= ny-2; j++) {
            u_new[i][j] = u[i][j] + dt * (viscosity * (diffusion_in_x(u[i+1][j], u[i][j], u[i-1][j], dx) + diffusion_in_y(u[i][j+1], u[i][j], u[i][j-1], dy)) - advection_in_y(v[i][j], u[i][j+1], u[i][j-1], dy) - advection_in_x(u[i][j], u[i+1][j], u[i-1][j], dx));
            v_new[i][j] = v[i][j] + dt * (viscosity * (diffusion_in_x(v[i+1][j], v[i][j], v[i-1][j], dx) + diffusion_in_y(v[i][j+1], v[i][j], v[i][j-1], dy)) - advection_in_y(v[i][j], v[i][j+1], v[i][j-1], dy) - advection_in_x(u[i][j], v[i+1][j], v[i-1][j], dx));
        }
    }

    for (int i = 1; i <= nx-2; i++) {
        for (int j = 1; j <= ny-2; j++) {
            u[i][j] = u_new[i][j];
            v[i][j] = v_new[i][j];
        }
    }
}

int main() {
    cfl_compute_time_spacing(DX, DY, VISCOSITY);
    initialize_grid(GRID_POINTS_X, GRID_POINTS_Y);

    initialize_perturbation(GRID_POINTS_X, GRID_POINTS_Y);

    float t = 0.0f;

    InitWindow(WINDOW_W, WINDOW_H, "Navier-Stokes");
    SetTargetFPS(60);

    while (!WindowShouldClose() && t < TIME) {
        update_velocity(DX, DY, time_spacing, GRID_POINTS_X, GRID_POINTS_Y, VISCOSITY);
        t += time_spacing;
        time_step++;

        BeginDrawing();
            ClearBackground(BLACK);
            render_grid(GRID_POINTS_X, GRID_POINTS_Y, u);
        EndDrawing();
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
            render_grid(GRID_POINTS_X, GRID_POINTS_Y, u);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
