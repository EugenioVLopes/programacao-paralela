#define _GNU_SOURCE
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

#define TOTAL_ELEMENTOS_PADRAO 5000000L

static double obter_tempo_segundos(void) {
    struct timespec tempo_atual;
    clock_gettime(CLOCK_MONOTONIC, &tempo_atual);
    return (double)tempo_atual.tv_sec + (double)tempo_atual.tv_nsec / 1e9;
}

int main(int argc, char *argv[]) {
    long long total_elementos = argc > 1 ? atoll(argv[1]) : TOTAL_ELEMENTOS_PADRAO;

    long long dentro_circulo = 0;

    double tempo = obter_tempo_segundos();

    #pragma omp parallel for
    for (long long i = 0; i < total_elementos; i++) {
        unsigned int seed = (unsigned int)(i + 1);
        double x = 2.0 * (double)rand_r(&seed) / (double)RAND_MAX - 1.0;
        double y = 2.0 * (double)rand_r(&seed) / (double)RAND_MAX - 1.0;

        if ((x * x + y * y) <= 1.0) {
            #pragma omp critical
            {
                dentro_circulo++;
            }
        }
    }

    double pi = 4.0 * dentro_circulo / total_elementos;
    double erro = fabs(pi - M_PI);

    tempo = obter_tempo_segundos() - tempo;

    printf("PI = %.10f\nErro = %.10e\nTempo = %.9f segundos\n", pi, erro, tempo);

    return 0;
}