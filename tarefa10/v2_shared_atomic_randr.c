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

    long hits = 0;
    double tempo = agora();

    #pragma omp parallel shared(hits, N) default(none)
    {

        #pragma omp for
        for (long i = 0; i < N; i++) {
            unsigned int seed = 12345u + (unsigned int)(i * 2654435761u);
            double x = (double)rand_r(&seed) / (double)RAND_MAX * 2.0 - 1.0;
            double y = (double)rand_r(&seed) / (double)RAND_MAX * 2.0 - 1.0;
            if (x * x + y * y <= 1.0) {
                #pragma omp atomic
                hits++;
            }
        }
    }

    tempo = agora() - tempo;
    printf("v2 shared+atomic+rand_r: pi=%.6f tempo=%.4f s hits=%ld\n",
           4.0 * hits / N, tempo, hits);
    return 0;
}
