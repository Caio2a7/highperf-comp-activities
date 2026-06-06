#include <stdio.h>
#include <math.h>
#include <omp.h>
#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define VISCOSITY 0.1f
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

void render_grid(int nx, int ny, int wx, int wy, float *grid) {
    float min_val = grid[IDX(0,0)], max_val = grid[IDX(0,0)];
    // Stage 1 - Iteração sobre os pontos da malha para descobrir seus valores máximos de velocidade (importante para determinar a coloração)
    for (int i = 0; i < nx; i++)
        for (int j = 0; j < ny; j++) {
            if (grid[IDX(i,j)] < min_val) min_val = grid[IDX(i,j)];
            if (grid[IDX(i,j)] > max_val) max_val = grid[IDX(i,j)];
        }
    float range = max_val - min_val;
    if (range == 0.0f) range = 1.0f;

    Color *pixels = (Color*)img.data;
    // Iteração sobre os pontos da malha
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            float t = (grid[IDX(i,j)] - min_val) / range;
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
            u_curr[IDX(i,j)] = 4.0f;
            v_curr[IDX(i,j)] = 0.0f;
            u_next[IDX(i,j)] = 4.0f;
            v_next[IDX(i,j)] = 0.0f;
        }
}

void centric_circle_perturbation(int nx, int ny) {
    float cx = LX / 2.0f;
    float cy = LY / 2.0f;
    float radius = LX * 0.1f;
    for (int i = 0; i < nx; i++) {
        for (int j = 0; j < ny; j++) {
            float x = i * DX;
            float y = j * DY;
            float dist = sqrtf((x - cx)*(x - cx) + (y - cy)*(y - cy));
            if (dist < radius) {          
                u_curr[IDX(i,j)] += 0.5f;
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

    int n_vortices = 80;
    float vortex_radius = LX * 0.03f; // vórtices pequenos

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

void update_velocity(float dx, float dy, float dt, int nx, int ny, float viscosity) {
    #pragma omp parallel for collapse(2)
    for (int i = 1; i <= nx-2; i++) {
        for (int j = 1; j <= ny-2; j++) {
            u_next[IDX(i,j)] = u_curr[IDX(i,j)] + dt * (
                viscosity * (
                    diffusion_in_x(u_curr[IDX(i+1,j)], u_curr[IDX(i,j)], u_curr[IDX(i-1,j)], dx) +
                    diffusion_in_y(u_curr[IDX(i,j+1)], u_curr[IDX(i,j)], u_curr[IDX(i,j-1)], dy)
                )
                - advection_in_x(u_curr[IDX(i,j)], u_curr[IDX(i+1,j)], u_curr[IDX(i-1,j)], dx)
                - advection_in_y(v_curr[IDX(i,j)], u_curr[IDX(i,j+1)], u_curr[IDX(i,j-1)], dy)
            );
            v_next[IDX(i,j)] = v_curr[IDX(i,j)] + dt * (
                viscosity * (
                    diffusion_in_x(v_curr[IDX(i+1,j)], v_curr[IDX(i,j)], v_curr[IDX(i-1,j)], dx) +
                    diffusion_in_y(v_curr[IDX(i,j+1)], v_curr[IDX(i,j)], v_curr[IDX(i,j-1)], dy)
                )
                - advection_in_x(u_curr[IDX(i,j)], v_curr[IDX(i+1,j)], v_curr[IDX(i-1,j)], dx)
                - advection_in_y(v_curr[IDX(i,j)], v_curr[IDX(i,j+1)], v_curr[IDX(i,j-1)], dy)
            );
        }
    }

    // swap grids
    float *tmp;
    tmp    = u_curr;  u_curr = u_next;  u_next = tmp;
    tmp    = v_curr;  v_curr = v_next;  v_next = tmp;
}

int main() {
    cfl_compute_time_spacing(DX, DY, VISCOSITY);
    initialize_grid(GRID_POINTS_X, GRID_POINTS_Y);
    
    chaotic_perturbation(GRID_POINTS_X, GRID_POINTS_Y);
    
    float t = 0.0f;
    
    InitWindow(WINDOW_W, WINDOW_H, "Navier-Stokes");
    SetTargetFPS(60);

    // inicializa imagem em CPU e textura em GPU uma única vez
    img = GenImageColor(GRID_POINTS_X, GRID_POINTS_Y, BLACK);
    tex = LoadTextureFromImage(img);
    
    // roda N passos de simulação por frame para compensar time_spacing pequeno em malhas densas
    int steps_per_frame = (int)(0.01f / time_spacing) + 1;

    while (!WindowShouldClose() && t < TIME) {
        BeginDrawing();
            ClearBackground(BLACK);
            render_grid(GRID_POINTS_X, GRID_POINTS_Y, WINDOW_W, WINDOW_H, u_curr);
        EndDrawing();
      
        for (int s = 0; s < steps_per_frame; s++) {
            update_velocity(DX, DY, time_spacing, GRID_POINTS_X, GRID_POINTS_Y, VISCOSITY);
            t += time_spacing;
        }

        time_step++;
    }
    while (!WindowShouldClose()) {
        BeginDrawing();
            render_grid(GRID_POINTS_X, GRID_POINTS_Y, WINDOW_W, WINDOW_H, u_curr);
        EndDrawing();
    }

    UnloadTexture(tex);
    UnloadImage(img);
    CloseWindow();
    return 0;
}
