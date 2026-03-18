#include <stdio.h>
#include <time.h>
#include <string.h>

#define TEST_1_SIZE 10
#define TEST_2_SIZE 1000
#define TEST_3_SIZE 10000

typedef void (*mat_mul_fn)(int, int, int[][*], int[], int[]);

int mat_a[TEST_1_SIZE][TEST_1_SIZE];
int mat_b[TEST_2_SIZE][TEST_2_SIZE];
int mat_c[TEST_3_SIZE][TEST_3_SIZE];
int vec_a[TEST_1_SIZE];
int vec_b[TEST_2_SIZE];
int vec_c[TEST_3_SIZE];
int res_a[TEST_1_SIZE];
int res_b[TEST_2_SIZE];
int res_c[TEST_3_SIZE];

double time_elapsed_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 +
           (end.tv_nsec - start.tv_nsec) / 1e6;
}

void mat_vec_init(int rows, int cols, int mat[rows][cols], int vec[cols], int result[rows]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            mat[i][j] = 1;
        }
    }
    for (int i = 0; i < cols; i++) {
        vec[i] = 1;
    }
    for (int i = 0; i < rows; i++) {
        result[i] = 0;
    }
}

void mat_vector_row_mul(int rows, int cols, int mat[rows][cols], int vec[cols], int result[rows]) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            result[i] += mat[i][j] * vec[j];
        }
    }
}

void mat_vector_col_mul(int rows, int cols, int mat[rows][cols], int vec[cols], int result[rows]) {
    for (int j = 0; j < cols; j++) {
        for (int i = 0; i < rows; i++) {
            result[i] += mat[i][j] * vec[j];
        }
    }
}

void mat_mul_benchmark(mat_mul_fn fn, int rows, int cols, int mat[rows][cols], int vec[cols], int result[rows]) {
    struct timespec t_start, t_end;
    memset(result, 0, rows * sizeof(int));
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    fn(rows, cols, mat, vec, result);
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    printf("- Matriz %dx%d: \n", rows, cols);
    printf("Tempo:: %.4f ms\n\n", time_elapsed_ms(t_start, t_end));
}

int main(void) {
    mat_vec_init(TEST_1_SIZE, TEST_1_SIZE, mat_a, vec_a, res_a);
    mat_vec_init(TEST_2_SIZE, TEST_2_SIZE, mat_b, vec_b, res_b);
    mat_vec_init(TEST_3_SIZE, TEST_3_SIZE, mat_c, vec_c, res_c);

    printf("--- Benchmark Multiplicacao Linha externa - Coluna interna ---\n\n");
    mat_mul_benchmark(mat_vector_row_mul, TEST_1_SIZE, TEST_1_SIZE, mat_a, vec_a, res_a);
    mat_mul_benchmark(mat_vector_row_mul, TEST_2_SIZE, TEST_2_SIZE, mat_b, vec_b, res_b);
    mat_mul_benchmark(mat_vector_row_mul, TEST_3_SIZE, TEST_3_SIZE, mat_c, vec_c, res_c);

    printf("--- Benchmark Multiplicacao Coluna externa - Linha interna ---\n\n");
    mat_mul_benchmark(mat_vector_col_mul, TEST_1_SIZE, TEST_1_SIZE, mat_a, vec_a, res_a);
    mat_mul_benchmark(mat_vector_col_mul, TEST_2_SIZE, TEST_2_SIZE, mat_b, vec_b, res_b);
    mat_mul_benchmark(mat_vector_col_mul, TEST_3_SIZE, TEST_3_SIZE, mat_c, vec_c, res_c);

    return 0;
}
