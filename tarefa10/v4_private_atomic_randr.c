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

int main(void) {
    const long N = 2000000L;

    long total = 0;
    double tempo = agora();

    #pragma omp parallel shared(total, N) default(none)
    {
        long local = 0;

        #pragma omp for
        for (long i = 0; i < N; i++) {
            unsigned int seed = 12345u + (unsigned int)(i * 2654435761u);
            double x = (double)rand_r(&seed) / (double)RAND_MAX * 2.0 - 1.0;
            double y = (double)rand_r(&seed) / (double)RAND_MAX * 2.0 - 1.0;
            if (x * x + y * y <= 1.0) {
                local++;
            }
        }

        #pragma omp atomic
        total += local;
    }

    tempo = agora() - tempo;
    printf("v4 private+atomic-final+rand_r: pi=%.6f tempo=%.4f s hits=%ld\n",
           4.0 * total / N, tempo, total);
    return 0;
}
