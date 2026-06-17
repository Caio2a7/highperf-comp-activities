#include <stdio.h>
#include <math.h>
#include <omp.h>
#include "raylib.h"
#include <stdlib.h>

#define VISCOSITY 0.02f
#define TIME 10.0f
#define LX 20.0f
#define LY 20.0f
#define GRID_POINTS_X 800
#define GRID_POINTS_Y 800
#define DX (LX / (GRID_POINTS_X-1))
#define DY (LY / (GRID_POINTS_Y-1))
#define WINDOW_W 800
#define WINDOW_H 800
#define CELL_W (WINDOW_W / GRID_POINTS_X)
#define CELL_H (WINDOW_H / GRID_POINTS_Y)
// Formula para acessar matriz de forma row-major mesmo em caso de column-major: posição i*200 + j = u[i][j], sendo 200 o número de pontos ou o nx ou ny
#define IDX(i, j) ((i) * GRID_POINTS_Y + (j))
// Vetorização do Grid de pontos para fetch vide IDX
float u_buf_a[GRID_POINTS_X * GRID_POINTS_Y];
float u_buf_b[GRID_POINTS_X * GRID_POINTS_Y];
float v_buf_a[GRID_POINTS_X * GRID_POINTS_Y];
float v_buf_b[GRID_POINTS_X * GRID_POINTS_Y];
// Ponteiros para cópias entre grid antigo e grid atualizado no update_velocity
float *u_curr = u_buf_a;
float *u_next = u_buf_b;
float *v_curr = v_buf_a;
float *v_next = v_buf_b;

int time_step = 0;
float time_spacing = 1.0;

// Imagem em CPU e textura em GPU — render via 1 única draw call em vez de nx*ny DrawRectangles
Image img;
Texture2D tex;

void render_grid(int nx, int ny, int wx, int wy, float *u_grid, float *v_grid) {
    float max_val = 0.0001f;
    // Stage 1 - busca o maior valor de magnitude de velocidade do vetor (u,v)
    // magnitude = sqrt(u² + v²) representa a velocidade real do fluido em cada ponto
    #pragma omp parallel for collapse(2) schedule(static) reduction(max:max_val)
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++) {
            float mag = sqrtf(u_grid[IDX(i,j)]*u_grid[IDX(i,j)] + v_grid[IDX(i,j)]*v_grid[IDX(i,j)]);
            if (mag > max_val) max_val = mag;
        }

    Color *pixels = (Color*)img.data;
    // Iteração sobre os pontos da malha
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            float mag = sqrtf(u_grid[IDX(i,j)]*u_grid[IDX(i,j)] + v_grid[IDX(i,j)]*v_grid[IDX(i,j)]);
            float t = mag / max_val;
            // Stage 2 - Tradução do valor do ponto da malha (sua velocidade) para uma cor (1 - t) * 240
            // 240 é o máximo de coloração (azul) 0 é o mínimo vermelho e como t varia entre [0,1] temos esse range de cor
            // Stage 3 - Conversão da quantidade de pontos discretizados da malha para quadradinho de pixels desenhados na tela
            // Exp: Para 200x200 pontos numa tela 800x800 pixels teremos um quadradinho de 4x4 pixels para representar 1 ponto da malha  
            // Regra de 3 direta entre pontos da malha para pixels
            // Exp: Digamos para o ponto i = 3 da malha, para nx=200 e wx=200
            // 200 - 800
            //  3  -  x0
            // Sendo x0 a posição do pixel exato na tela aonde começa as coordenadas do quadradinho e por isso (i*wx) / nx que é basicamente (3*800)/200
            // pixels[j * nx + i] mapeia (i,j) da malha para posição linear na imagem (row-major de pixels)
            pixels[j * nx + i] = ColorFromHSV((1.0f - t) * 240.0f, 1.0f, 1.0f);
        }
    }

    // 1 upload CPU → GPU e 1 draw call escalando a textura para o tamanho da janela
    UpdateTexture(tex, img.data);
    DrawTexturePro(tex,
        (Rectangle){0, 0, nx, ny},
        (Rectangle){0, 0, wx, wy},
        (Vector2){0, 0}, 0, WHITE);
}

void initialize_grid(int nx, int ny) {
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++) {
            // fluido em repouso — sem velocidade inicial em nenhuma direção
            u_curr[IDX(i,j)] = 0.0f;
            v_curr[IDX(i,j)] = 0.0f;
            u_next[IDX(i,j)] = 0.0f;
            v_next[IDX(i,j)] = 0.0f;
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
            float dist = sqrtf(dx*dx + dy*dy);
            // perturbação radial — cada ponto dentro do círculo recebe velocidade
            // apontando para fora do centro, simulando uma pedra caindo na água
            if (dist < radius && dist > 0.001f) {
                float intensity = strength * (1.0f - dist / radius);
                u_curr[IDX(i,j)] += intensity * (dx / dist);
                v_curr[IDX(i,j)] += intensity * (dy / dist);
                u_next[IDX(i,j)]  = u_curr[IDX(i,j)];
                v_next[IDX(i,j)]  = v_curr[IDX(i,j)];
            }
        }
    }
}

void wave_perturbation(int nx, int ny) {
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            float x = i * DX;
            u_curr[IDX(i,j)] += 0.5f * fabsf(sinf(2.0f * M_PI * x / LX));
        }
    }
}

void chaotic_perturbation(int nx, int ny) {
    srand(42); // semente fixa para reprodutibilidade — troque para srand(time(NULL)) se quiser diferente a cada execução

    int n_vortices = 70;
    float vortex_radius = LX * 0.05f; // vórtices pequenos

    for (int k = 0; k < n_vortices; k++) {
        // posição e rotação aleatórias
        float cx = ((float)rand() / RAND_MAX) * LX;
        float cy = ((float)rand() / RAND_MAX) * LY;
        float sign = (rand() % 2 == 0) ? 1.0f : -1.0f;
        float strength = 0.2f + ((float)rand() / RAND_MAX) * 0.8f; // intensidade variável entre 0.2 e 1.0
        
        for (int i = 0; i < nx; i++) {
            for (int j = 0; j < ny; j++) {
                float x = i * DX;
                float y = j * DY;
                float dx = x - cx;
                float dy = y - cy;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist < vortex_radius) {
                    float du = sign * (-dy / (dist + 0.1f)) * strength;
                    float dv = sign * ( dx / (dist + 0.1f)) * strength;
                    // magnitude garante valores >= 0
                    u_curr[IDX(i,j)] += sqrtf(du*du + dv*dv);
                }
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


int main() {
    omp_set_num_threads(20);

    cfl_compute_time_spacing(DX, DY, VISCOSITY);
    initialize_grid(GRID_POINTS_X, GRID_POINTS_Y);
    chaotic_perturbation(GRID_POINTS_X, GRID_POINTS_Y);
    // centric_circle_perturbation(GRID_POINTS_X, GRID_POINTS_Y);

    InitWindow(WINDOW_W, WINDOW_H, "Navier-Stokes");
    SetTargetFPS(60);
    img = GenImageColor(GRID_POINTS_X, GRID_POINTS_Y, BLACK);
    tex = LoadTextureFromImage(img);

    float t = 0.0f;
    double t0, t1;
    int steps_per_frame = (int)(0.01f / time_spacing) + 1;
    int running = 1;

    #pragma omp parallel shared(t, time_step, running, u_curr, u_next, v_curr, v_next)
    {
        while (running && t < TIME) {

            // render — obrigatoriamente thread 0 (contexto OpenGL vinculado à thread principal)
            #pragma omp master
            {
                BeginDrawing();
                    ClearBackground(BLACK);
                    render_grid(GRID_POINTS_X, GRID_POINTS_Y, WINDOW_W, WINDOW_H, u_curr, v_curr);
                EndDrawing();
                running = !WindowShouldClose();
            }
            #pragma omp barrier

            for (int s = 0; s < steps_per_frame; s++) {
                #pragma omp master
                { t0 = omp_get_wtime(); }
                #pragma omp barrier
                // schedule(static, GRID_POINTS_Y) alinha chunks a linhas inteiras
                // É neste loop duplo que ocorre de fato o calculo das mudanças de velocidade em decorrer do tempo das particulas do fluido vide termo de navier-stokes para viscosidade 2d e discreto
                #pragma omp for collapse(2) schedule(static, GRID_POINTS_X)
                for (int i = 1; i <= GRID_POINTS_X-2; i++) {
                    for (int j = 1; j <= GRID_POINTS_Y-2; j++) {
                        u_next[IDX(i,j)] = u_curr[IDX(i,j)] + time_spacing * (
                            VISCOSITY * (
                                diffusion_in_x(u_curr[IDX(i+1,j)], u_curr[IDX(i,j)], u_curr[IDX(i-1,j)], DX) +
                                diffusion_in_y(u_curr[IDX(i,j+1)], u_curr[IDX(i,j)], u_curr[IDX(i,j-1)], DY)
                            )
                            - advection_in_x(u_curr[IDX(i,j)], u_curr[IDX(i+1,j)], u_curr[IDX(i-1,j)], DX)
                            - advection_in_y(v_curr[IDX(i,j)], u_curr[IDX(i,j+1)], u_curr[IDX(i,j-1)], DY)
                        );
                        v_next[IDX(i,j)] = v_curr[IDX(i,j)] + time_spacing * (
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
                    // swap grids
                    float *tmp;
                    tmp    = u_curr;  u_curr = u_next;  u_next = tmp;
                    tmp    = v_curr;  v_curr = v_next;  v_next = tmp;

                    t1 = omp_get_wtime();
                    printf("Grid update in: %0.5lfs\n", t1 - t0);
                    t += time_spacing;
                }
                #pragma omp barrier
            }

            #pragma omp master
            { time_step++; }
            #pragma omp barrier
        }
    }

    // loop de render final após simulação concluída
    while (!WindowShouldClose()) {
        BeginDrawing();
          render_grid(GRID_POINTS_X, GRID_POINTS_Y, WINDOW_W, WINDOW_H, u_curr, v_curr);
        EndDrawing();
    }

    UnloadTexture(tex);
    UnloadImage(img);
    CloseWindow();
    return 0;
}
