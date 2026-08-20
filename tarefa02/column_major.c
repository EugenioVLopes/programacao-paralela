#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Acesso por colunas (column-major)
 * y[i] = sum_j A[i][j] * x[j]
 * Laço externo varia a coluna j, interno varia a linha i
 * A[i * N + j] com j fixo e i variando salta N posições (8N bytes)
 */

static double agora(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(int argc, char *argv[]) {
    int N = 2000;
    if (argc > 1) N = atoi(argv[1]);

    double *A = malloc((size_t)N * N * sizeof(double));
    double *x = malloc(N * sizeof(double));
    double *y = calloc(N, sizeof(double));

    for (int i = 0; i < N * N; i++) A[i] = 1.0;
    for (int i = 0; i < N; i++) x[i] = 1.0;

    double inicio = agora();

    for (int j = 0; j < N; j++) {
        for (int i = 0; i < N; i++)
            y[i] += A[i * N + j] * x[j];
    }

    double fim = agora();
    double tempo = fim - inicio;
    printf("Column-major  N=%d  tempo=%.6f s  y[0]=%.1f\n", N, tempo, y[0]);

    free(A); free(x); free(y);
    return 0;
}
