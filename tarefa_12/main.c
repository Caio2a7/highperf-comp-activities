#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <stdlib.h>

#define VISCOSITY 0.05f
#define TIME 10.0f
#define LX 20.0f
#define LY 20.0f

int GRID_POINTS_X;
int GRID_POINTS_Y;
float DX;
float DY;

#define IDX(i, j) ((i) * GRID_POINTS_Y + (j))

float *u_buf_a;
float *u_buf_b;
float *v_buf_a;
float *v_buf_b;

float *u_curr;
float *u_next;
float *v_curr;
float *v_next;

float time_spacing = 1.0f;

void initialize_grid(int nx, int ny) {
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++) {
            u_curr[IDX(i, j)] = 0.0f;
            v_curr[IDX(i, j)] = 0.0f;
            u_next[IDX(i, j)] = 0.0f;
            v_next[IDX(i, j)] = 0.0f;
        }
}

void centric_circle_perturbation(int nx, int ny) {
    float cx = LX / 2.0f;
    float cy = LY / 2.0f;
    float radius = LX * 0.08f;
    float strength = 1.5f;

    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            float x = i * DX;
            float y = j * DY;
            float dx = x - cx;
            float dy = y - cy;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist < radius && dist > 0.001f) {
                float intensity = strength * (1.0f - dist / radius);
                u_curr[IDX(i, j)] += intensity * (dx / dist);
                v_curr[IDX(i, j)] += intensity * (dy / dist);
                u_next[IDX(i, j)] = u_curr[IDX(i, j)];
                v_next[IDX(i, j)] = v_curr[IDX(i, j)];
            }
        }
    }
}

void wave_perturbation(int nx, int ny) {
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++) {
            float x = i * DX;
            u_curr[IDX(i, j)] += 0.5f * fabsf(sinf(2.0f * M_PI * x / LX));
        }
}

void chaotic_perturbation(int nx, int ny) {
    srand(42);
    int n_vortices = 70;
    float vortex_radius = LX * 0.05f;

    for (int k = 0; k < n_vortices; k++) {
        float cx = ((float)rand() / RAND_MAX) * LX;
        float cy = ((float)rand() / RAND_MAX) * LY;
        float sign = (rand() % 2 == 0) ? 1.0f : -1.0f;
        float strength = 0.2f + ((float)rand() / RAND_MAX) * 0.8f;

        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                float x = i * DX;
                float y = j * DY;
                float dx = x - cx;
                float dy = y - cy;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist < vortex_radius) {
                    float du = sign * (-dy / (dist + 0.1f)) * strength;
                    float dv = sign * (dx / (dist + 0.1f)) * strength;
                    u_curr[IDX(i, j)] += sqrtf(du * du + dv * dv);
                }
            }
        }
    }
}

void cfl_compute_time_spacing(float dx, float dy, float viscosity) {
    time_spacing = ((dx * dx) * (dy * dy)) / (2 * viscosity * ((dx * dx) + (dy * dy)));
}

float diffusion_in_x(float first_term, float second_term, float third_term, float dx) {
    return (first_term - 2 * second_term + third_term) / (dx * dx);
}

float diffusion_in_y(float first_term, float second_term, float third_term, float dy) {
    return (first_term - 2 * second_term + third_term) / (dy * dy);
}

float advection_in_x(float first_term, float second_term, float third_term, float dx) {
    return (first_term * (second_term - third_term)) / (dx * 2);
}

float advection_in_y(float first_term, float second_term, float third_term, float dy) {
    return (first_term * (second_term - third_term)) / (dy * 2);
}

void print_timing_summary(int steps, double total_wall, double total_step_time) {
    printf("\n========== Timing Summary ==========\n");
    printf("Threads         : %d\n", omp_get_max_threads());
    printf("Total steps     : %d\n", steps);
    printf("Total wall time : %.4f s\n", total_wall);
    printf("Total step time : %.4f s\n", total_step_time);
    printf("Avg step time   : %.6f s\n", steps > 0 ? total_step_time / steps : 0.0);
    printf("Steps/second    : %.2f\n", total_wall > 0.0 ? steps / total_wall : 0.0);
    printf("====================================\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <GRID_POINTS_X> <GRID_POINTS_Y>\n", argv[0]);
        return 1;
    }

    GRID_POINTS_X = atoi(argv[1]);
    GRID_POINTS_Y = atoi(argv[2]);
    DX = LX / (GRID_POINTS_X - 1);
    DY = LY / (GRID_POINTS_Y - 1);

    u_buf_a = malloc(GRID_POINTS_X * GRID_POINTS_Y * sizeof(float));
    u_buf_b = malloc(GRID_POINTS_X * GRID_POINTS_Y * sizeof(float));
    v_buf_a = malloc(GRID_POINTS_X * GRID_POINTS_Y * sizeof(float));
    v_buf_b = malloc(GRID_POINTS_X * GRID_POINTS_Y * sizeof(float));

    u_curr = u_buf_a;
    u_next = u_buf_b;
    v_curr = v_buf_a;
    v_next = v_buf_b;

    cfl_compute_time_spacing(DX, DY, VISCOSITY);
    initialize_grid(GRID_POINTS_X, GRID_POINTS_Y);
    chaotic_perturbation(GRID_POINTS_X, GRID_POINTS_Y);

    float t = 0.0f;
    int total_steps = 0;
    double total_step_time = 0.0;

    double sim_start = omp_get_wtime();

    #pragma omp parallel shared(t, total_steps, total_step_time, u_curr, u_next, v_curr, v_next)
    {
        while (t < TIME) {
            double t0, t1;

            #pragma omp master
            { t0 = omp_get_wtime(); }
            #pragma omp barrier

            #pragma omp for collapse(2) schedule(static, GRID_POINTS_X)
            for (int i = 1; i <= GRID_POINTS_X - 2; i++) {
                for (int j = 1; j <= GRID_POINTS_Y - 2; j++) {
                    u_next[IDX(i, j)] = u_curr[IDX(i, j)] + time_spacing * (
                        VISCOSITY * (
                            diffusion_in_x(u_curr[IDX(i+1,j)], u_curr[IDX(i,j)], u_curr[IDX(i-1,j)], DX) +
                            diffusion_in_y(u_curr[IDX(i,j+1)], u_curr[IDX(i,j)], u_curr[IDX(i,j-1)], DY)
                        )
                        - advection_in_x(u_curr[IDX(i,j)], u_curr[IDX(i+1,j)], u_curr[IDX(i-1,j)], DX)
                        - advection_in_y(v_curr[IDX(i,j)], u_curr[IDX(i,j+1)], u_curr[IDX(i,j-1)], DY)
                    );
                    v_next[IDX(i, j)] = v_curr[IDX(i, j)] + time_spacing * (
                        VISCOSITY * (
                            diffusion_in_x(v_curr[IDX(i+1,j)], v_curr[IDX(i,j)], v_curr[IDX(i-1,j)], DX) +
                            diffusion_in_y(v_curr[IDX(i,j+1)], v_curr[IDX(i,j)], v_curr[IDX(i,j-1)], DY)
                        )
                        - advection_in_x(u_curr[IDX(i,j)], v_curr[IDX(i+1,j)], v_curr[IDX(i-1,j)], DX)
                        - advection_in_y(v_curr[IDX(i,j)], v_curr[IDX(i,j+1)], v_curr[IDX(i,j-1)], DY)
                    );
                }
            }

            #pragma omp master
            {
                float *tmp;
                tmp = u_curr; u_curr = u_next; u_next = tmp;
                tmp = v_curr; v_curr = v_next; v_next = tmp;

                t1 = omp_get_wtime();
                total_step_time += (t1 - t0);
                total_steps++;
                t += time_spacing;

                if (total_steps % 1000 == 0)
                    printf("Step %6d | sim_t=%.4f | step=%.6f s\n", total_steps, t, t1 - t0);
            }
            #pragma omp barrier
        }
    }

    double sim_end = omp_get_wtime();
    print_timing_summary(total_steps, sim_end - sim_start, total_step_time);

    free(u_buf_a);
    free(u_buf_b);
    free(v_buf_a);
    free(v_buf_b);

    return 0;
}
