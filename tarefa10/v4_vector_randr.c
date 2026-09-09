#define _POSIX_C_SOURCE 199309L

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double agora(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(int argc, char *argv[]) {
    long N = (argc > 1) ? atol(argv[1]) : 5000000L;
    if (N <= 0) {
        fprintf(stderr, "Uso: %s [Npontos]\n", argv[0]);
        return 1;
    }

    int P = omp_get_max_threads();
    long *vec = calloc((size_t)P, sizeof(long));
    if (vec == NULL) {
        fprintf(stderr, "Falha ao alocar vetor de %d contadores\n", P);
        return 1;
    }

    double tempo = agora();

    #pragma omp parallel shared(vec, N) default(none)
    {
        int tid = omp_get_thread_num();
        long local = 0;

    #pragma omp for
        for (long i = 0; i < N; i++) {
            unsigned int seed = 12345u + (unsigned int)(i * 2654435761u);
            double x = (double)rand_r(&seed) / (double)RAND_MAX * 2.0 - 1.0;
            double y = (double)rand_r(&seed) / (double)RAND_MAX * 2.0 - 1.0;
            if (x * x + y * y <= 1.0) local++;
        }

        vec[tid] = local;
    }

    long total = 0;
    for (int t = 0; t < P; t++) total += vec[t];

    tempo = agora() - tempo;
    printf("v4 vector-per-thread+rand_r: pi=%.6f tempo=%.4f s hits=%ld\n",
           4.0 * total / N, tempo, total);

    free(vec);
    return 0;
}
