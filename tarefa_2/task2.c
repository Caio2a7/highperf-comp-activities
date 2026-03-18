#include <stdio.h>
#include <time.h>

#define VECTOR_SIZE 840000

int vector[VECTOR_SIZE];

typedef int (*vector_sum_fn)(int, int[]);

double time_elapsed_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 +
           (end.tv_nsec - start.tv_nsec) / 1e6;
}

void vector_init(int size, int vector[size]) {
    for (int i = 0; i < size; i++)
        vector[i] = 10;
}

int cumulative_vector_sum(int size, int vector[size]) {
    int sum = 0;
    for (int i = 0; i < size; i++)
        sum += vector[i];
    return sum;
}

int sum_2vars(int size, int vector[size]) {
    int s0 = 0, s1 = 0;
    for (int i = 0; i < size; i += 2) {
        s0 += vector[i];
        s1 += vector[i + 1];
    }
    return s0 + s1;
}

int sum_3vars(int size, int vector[size]) {
    int s0 = 0, s1 = 0, s2 = 0;
    for (int i = 0; i < size; i += 3) {
        s0 += vector[i];
        s1 += vector[i + 1];
        s2 += vector[i + 2];
    }
    return s0 + s1 + s2;
}

int sum_4vars(int size, int vector[size]) {
    int s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    for (int i = 0; i < size; i += 4) {
        s0 += vector[i];
        s1 += vector[i + 1];
        s2 += vector[i + 2];
        s3 += vector[i + 3];
    }
    return s0 + s1 + s2 + s3;
}

int sum_5vars(int size, int vector[size]) {
    int s0 = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0;
    for (int i = 0; i < size; i += 5) {
        s0 += vector[i];
        s1 += vector[i + 1];
        s2 += vector[i + 2];
        s3 += vector[i + 3];
        s4 += vector[i + 4];
    }
    return s0 + s1 + s2 + s3 + s4;
}

int sum_6vars(int size, int vector[size]) {
    int s0 = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0;
    for (int i = 0; i < size; i += 6) {
        s0 += vector[i];
        s1 += vector[i + 1];
        s2 += vector[i + 2];
        s3 += vector[i + 3];
        s4 += vector[i + 4];
        s5 += vector[i + 5];
    }
    return s0 + s1 + s2 + s3 + s4 + s5;
}

int sum_7vars(int size, int vector[size]) {
    int s0 = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0, s6 = 0;
    for (int i = 0; i < size; i += 7) {
        s0 += vector[i];
        s1 += vector[i + 1];
        s2 += vector[i + 2];
        s3 += vector[i + 3];
        s4 += vector[i + 4];
        s5 += vector[i + 5];
        s6 += vector[i + 6];
    }
    return s0 + s1 + s2 + s3 + s4 + s5 + s6;
}

int sum_8vars(int size, int vector[size]) {
    int s0 = 0, s1 = 0, s2 = 0, s3 = 0,
        s4 = 0, s5 = 0, s6 = 0, s7 = 0;
    for (int i = 0; i < size; i += 8) {
        s0 += vector[i];
        s1 += vector[i + 1];
        s2 += vector[i + 2];
        s3 += vector[i + 3];
        s4 += vector[i + 4];
        s5 += vector[i + 5];
        s6 += vector[i + 6];
        s7 += vector[i + 7];
    }
    return s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7;
}

int sum_12vars(int size, int vector[size]) {
    int s0  = 0, s1  = 0, s2  = 0, s3  = 0,
        s4  = 0, s5  = 0, s6  = 0, s7  = 0,
        s8  = 0, s9  = 0, s10 = 0, s11 = 0;
    for (int i = 0; i < size; i += 12) {
        s0  += vector[i];
        s1  += vector[i + 1];
        s2  += vector[i + 2];
        s3  += vector[i + 3];
        s4  += vector[i + 4];
        s5  += vector[i + 5];
        s6  += vector[i + 6];
        s7  += vector[i + 7];
        s8  += vector[i + 8];
        s9  += vector[i + 9];
        s10 += vector[i + 10];
        s11 += vector[i + 11];
    }
    return s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8 + s9 + s10 + s11;
}

int sum_15vars(int size, int vector[size]) {
    int s0  = 0, s1  = 0, s2  = 0, s3  = 0,
        s4  = 0, s5  = 0, s6  = 0, s7  = 0,
        s8  = 0, s9  = 0, s10 = 0, s11 = 0,
        s12 = 0, s13 = 0, s14 = 0;
    for (int i = 0; i < size; i += 15) {
        s0  += vector[i];
        s1  += vector[i + 1];
        s2  += vector[i + 2];
        s3  += vector[i + 3];
        s4  += vector[i + 4];
        s5  += vector[i + 5];
        s6  += vector[i + 6];
        s7  += vector[i + 7];
        s8  += vector[i + 8];
        s9  += vector[i + 9];
        s10 += vector[i + 10];
        s11 += vector[i + 11];
        s12 += vector[i + 12];
        s13 += vector[i + 13];
        s14 += vector[i + 14];
    }
    return s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8 + s9 + s10 + s11 + s12 + s13 + s14;
}

int sum_20vars(int size, int vector[size]) {
    int s0  = 0, s1  = 0, s2  = 0, s3  = 0,
        s4  = 0, s5  = 0, s6  = 0, s7  = 0,
        s8  = 0, s9  = 0, s10 = 0, s11 = 0,
        s12 = 0, s13 = 0, s14 = 0, s15 = 0,
        s16 = 0, s17 = 0, s18 = 0, s19 = 0;
    for (int i = 0; i < size; i += 20) {
        s0  += vector[i];
        s1  += vector[i + 1];
        s2  += vector[i + 2];
        s3  += vector[i + 3];
        s4  += vector[i + 4];
        s5  += vector[i + 5];
        s6  += vector[i + 6];
        s7  += vector[i + 7];
        s8  += vector[i + 8];
        s9  += vector[i + 9];
        s10 += vector[i + 10];
        s11 += vector[i + 11];
        s12 += vector[i + 12];
        s13 += vector[i + 13];
        s14 += vector[i + 14];
        s15 += vector[i + 15];
        s16 += vector[i + 16];
        s17 += vector[i + 17];
        s18 += vector[i + 18];
        s19 += vector[i + 19];
    }
    return s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7 + s8 + s9 + s10 + s11 + s12 + s13 + s14 + s15 + s16 + s17 + s18 + s19;
}

void vector_sum_benchmark(const char *label, vector_sum_fn fn, int size, int vector[size]) {
    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    int result = fn(size, vector);
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    printf("%-30s sum: %-12d time: %.6f ms\n",
           label, result, time_elapsed_ms(t_start, t_end));
}

int main(void) {
    vector_init(VECTOR_SIZE, vector);

    printf("# Vector Sum Benchmark — size: %d #\n\n", VECTOR_SIZE);
    vector_sum_benchmark("cumulative (1 var)", cumulative_vector_sum, VECTOR_SIZE, vector);
    vector_sum_benchmark("2 vars",             sum_2vars,             VECTOR_SIZE, vector);
    vector_sum_benchmark("3 vars",             sum_3vars,             VECTOR_SIZE, vector);
    vector_sum_benchmark("4 vars",             sum_4vars,             VECTOR_SIZE, vector);
    vector_sum_benchmark("5 vars",             sum_5vars,             VECTOR_SIZE, vector);
    vector_sum_benchmark("6 vars",             sum_6vars,             VECTOR_SIZE, vector);
    vector_sum_benchmark("7 vars",             sum_7vars,             VECTOR_SIZE, vector);
    vector_sum_benchmark("8 vars",             sum_8vars,             VECTOR_SIZE, vector);
    vector_sum_benchmark("12 vars",            sum_12vars,            VECTOR_SIZE, vector);
    vector_sum_benchmark("15 vars",            sum_15vars,            VECTOR_SIZE, vector);
    vector_sum_benchmark("20 vars",            sum_20vars,            VECTOR_SIZE, vector);

    return 0;
}
