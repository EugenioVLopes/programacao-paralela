#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

#define TOTAL_ELEMENTOS_PADRAO 50000000L

static double obter_tempo_segundos(void) {
    struct timespec tempo_atual;
    clock_gettime(CLOCK_MONOTONIC, &tempo_atual);
    return (double)tempo_atual.tv_sec + (double)tempo_atual.tv_nsec / 1e9;
}

int main(int argc, char **argv) {
    long total_elementos = argc > 1 ? atol(argv[1]) : TOTAL_ELEMENTOS_PADRAO;
    if (total_elementos <= 0) {
        fprintf(stderr, "Uso: %s [total_elementos]\n", argv[0]);
        return 1;
    }

    double *A = malloc((size_t)total_elementos * sizeof(*A));
    double *B = malloc((size_t)total_elementos * sizeof(*B));
    double *C = malloc((size_t)total_elementos * sizeof(*C));
    double *V = malloc((size_t)total_elementos * sizeof(*V));

    if (!A || !B || !C || !V) {
        perror("malloc");
        return 1;
    }

    for (long i = 0; i < total_elementos; ++i) {
        A[i] = (double)i * 0.5;
        B[i] = (double)i * 0.25;
        V[i] = (double)i * 0.1;
    }

    int total_threads = omp_get_max_threads();

    double tempo_inicio_memoria = obter_tempo_segundos();

    #pragma omp parallel for
    for (long i = 0; i < total_elementos; ++i) {
        C[i] = A[i] + B[i];
    }

    double tempo_execucao_memoria = obter_tempo_segundos() - tempo_inicio_memoria;

    double tempo_inicio_cpu = obter_tempo_segundos();

    #pragma omp parallel for
    for (long i = 0; i < total_elementos; ++i) {
        double val = V[i];
        for (int j = 0; j < 50; ++j) {
            val = sqrt(val + j * 0.01) + sin(val);
        }
        V[i] = val;
    }

    double tempo_execucao_cpu = obter_tempo_segundos() - tempo_inicio_cpu;

    printf("N=%ld threads=%d memory_bound=%.9f cpu_bound=%.9f\n",
           total_elementos, total_threads, tempo_execucao_memoria, tempo_execucao_cpu);

    free(A); free(B); free(C); free(V);

    return 0;
}
