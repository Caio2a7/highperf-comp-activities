#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define NX 512
#define NY 512
#define DX 1.0
#define DY 1.0
#define DT 0.1
#define NU 0.1
#define STEPS 100
#define INIT_VEL 1.0
#define PERTURB  2.0
#define NUM_CONFIGS 7

static double u[NX][NY], u_new[NX][NY];
static double v[NX][NY], v_new[NX][NY];

void initialize() {
    for (int i = 0; i < NX; i++)
        for (int j = 0; j < NY; j++) { u[i][j] = INIT_VEL; v[i][j] = 0.0; }
    int cx = NX / 2, cy = NY / 2;
    for (int di = -3; di <= 3; di++)
        for (int dj = -3; dj <= 3; dj++) {
            double r = sqrt((double)(di*di + dj*dj));
            u[cx+di][cy+dj] = INIT_VEL + (PERTURB - INIT_VEL) * exp(-r);
        }
}

void apply_bc() {
    for (int i = 0; i < NX; i++) {
        u_new[0][i] = INIT_VEL; u_new[NX-1][i] = INIT_VEL;
        u_new[i][0] = INIT_VEL; u_new[i][NY-1] = INIT_VEL;
        v_new[0][i] = 0.0;      v_new[NX-1][i] = 0.0;
        v_new[i][0] = 0.0;      v_new[i][NY-1] = 0.0;
    }
    memcpy(u, u_new, sizeof(u));
    memcpy(v, v_new, sizeof(v));
}

void step_serial() {
    for (int i = 1; i < NX-1; i++)
        for (int j = 1; j < NY-1; j++) {
            u_new[i][j] = u[i][j] + DT*NU*((u[i+1][j]-2*u[i][j]+u[i-1][j])/(DX*DX)+(u[i][j+1]-2*u[i][j]+u[i][j-1])/(DY*DY));
            v_new[i][j] = v[i][j] + DT*NU*((v[i+1][j]-2*v[i][j]+v[i-1][j])/(DX*DX)+(v[i][j+1]-2*v[i][j]+v[i][j-1])/(DY*DY));
        }
    apply_bc();
}

void step_static_nc(int chunk) {
    #pragma omp parallel for schedule(static, chunk) default(none) firstprivate(chunk) shared(u,v,u_new,v_new)
    for (int i = 1; i < NX-1; i++)
        for (int j = 1; j < NY-1; j++) {
            u_new[i][j] = u[i][j] + DT*NU*((u[i+1][j]-2*u[i][j]+u[i-1][j])/(DX*DX)+(u[i][j+1]-2*u[i][j]+u[i][j-1])/(DY*DY));
            v_new[i][j] = v[i][j] + DT*NU*((v[i+1][j]-2*v[i][j]+v[i-1][j])/(DX*DX)+(v[i][j+1]-2*v[i][j]+v[i][j-1])/(DY*DY));
        }
    apply_bc();
}

void step_static_c(int chunk) {
    #pragma omp parallel for collapse(2) schedule(static, chunk) default(none) firstprivate(chunk) shared(u,v,u_new,v_new)
    for (int i = 1; i < NX-1; i++)
        for (int j = 1; j < NY-1; j++) {
            u_new[i][j] = u[i][j] + DT*NU*((u[i+1][j]-2*u[i][j]+u[i-1][j])/(DX*DX)+(u[i][j+1]-2*u[i][j]+u[i][j-1])/(DY*DY));
            v_new[i][j] = v[i][j] + DT*NU*((v[i+1][j]-2*v[i][j]+v[i-1][j])/(DX*DX)+(v[i][j+1]-2*v[i][j]+v[i][j-1])/(DY*DY));
        }
    apply_bc();
}

void step_dynamic(int chunk) {
    #pragma omp parallel for collapse(2) schedule(dynamic, chunk) default(none) firstprivate(chunk) shared(u,v,u_new,v_new)
    for (int i = 1; i < NX-1; i++)
        for (int j = 1; j < NY-1; j++) {
            u_new[i][j] = u[i][j] + DT*NU*((u[i+1][j]-2*u[i][j]+u[i-1][j])/(DX*DX)+(u[i][j+1]-2*u[i][j]+u[i][j-1])/(DY*DY));
            v_new[i][j] = v[i][j] + DT*NU*((v[i+1][j]-2*v[i][j]+v[i-1][j])/(DX*DX)+(v[i][j+1]-2*v[i][j]+v[i][j-1])/(DY*DY));
        }
    apply_bc();
}

void step_guided() {
    #pragma omp parallel for collapse(2) schedule(guided) default(none) shared(u,v,u_new,v_new)
    for (int i = 1; i < NX-1; i++)
        for (int j = 1; j < NY-1; j++) {
            u_new[i][j] = u[i][j] + DT*NU*((u[i+1][j]-2*u[i][j]+u[i-1][j])/(DX*DX)+(u[i][j+1]-2*u[i][j]+u[i][j-1])/(DY*DY));
            v_new[i][j] = v[i][j] + DT*NU*((v[i+1][j]-2*v[i][j]+v[i-1][j])/(DX*DX)+(v[i][j+1]-2*v[i][j]+v[i][j-1])/(DY*DY));
        }
    apply_bc();
}

void step_auto() {
    #pragma omp parallel for collapse(2) schedule(auto) default(none) shared(u,v,u_new,v_new)
    for (int i = 1; i < NX-1; i++)
        for (int j = 1; j < NY-1; j++) {
            u_new[i][j] = u[i][j] + DT*NU*((u[i+1][j]-2*u[i][j]+u[i-1][j])/(DX*DX)+(u[i][j+1]-2*u[i][j]+u[i][j-1])/(DY*DY));
            v_new[i][j] = v[i][j] + DT*NU*((v[i+1][j]-2*v[i][j]+v[i-1][j])/(DX*DX)+(v[i][j+1]-2*v[i][j]+v[i][j-1])/(DY*DY));
        }
    apply_bc();
}

double calc_rms() {
    double diff = 0.0;
    for (int i = 0; i < NX; i++)
        for (int j = 0; j < NY; j++)
            diff += (u[i][j] - INIT_VEL) * (u[i][j] - INIT_VEL);
    return diff / (NX * NY);
}

double run_serial() {
    initialize();
    double t0 = omp_get_wtime();
    for (int t = 0; t < STEPS; t++) step_serial();
    return omp_get_wtime() - t0;
}

double run_config(int cfg) {
    initialize();
    double t0 = omp_get_wtime();
    for (int t = 0; t < STEPS; t++) {
        switch (cfg) {
            case 0: step_static_nc(1);  break;
            case 1: step_static_nc(16); break;
            case 2: step_static_c(1);   break;
            case 3: step_static_c(16);  break;
            case 4: step_dynamic(16);   break;
            case 5: step_guided();      break;
            case 6: step_auto();        break;
        }
    }
    return omp_get_wtime() - t0;
}

int main() {
    const char *labels[NUM_CONFIGS] = {
        "static(1)  sem collapse",
        "static(16) sem collapse",
        "static(1)  com collapse(2)",
        "static(16) com collapse(2)",
        "dynamic(16)+ collapse(2)",
        "guided     + collapse(2)",
        "auto       + collapse(2)",
    };

    printf("Threads %d | Grade: %dx%d | Passos: %d | NU=%.2f | DT=%.2f\n\n", omp_get_max_threads(), NX, NY, STEPS, NU, DT);

    double t_serial = run_serial();
    double rms_ref  = calc_rms();

    printf("%-30s %10s %10s %10s\n", "Configuracao", "Tempo(s)", "Speedup", "RMS");
    printf("%-30s %10s %10s %10s\n", "------------------------------", "----------", "----------", "----------");
    printf("%-30s %10.4f %10s %10.2e\n", "serial", t_serial, "1.00x", rms_ref);

    for (int c = 0; c < NUM_CONFIGS; c++) {
        double t   = run_config(c);
        double rms = calc_rms();
        printf("%-30s %10.4f %9.2fx %10.2e\n", labels[c], t, t_serial / t, rms);
    }

    return 0;
}
