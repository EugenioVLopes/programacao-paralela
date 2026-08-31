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

    #pragma omp parallel default(none) shared(total_elementos, dentro_circulo)
    {
        long long local_dentro = 0;
        unsigned int seed = 123456789U ^ (unsigned int)omp_get_thread_num();

        #pragma omp for
        for (long long i = 0; i < total_elementos; i++) {
            double x = 2.0 * (double)rand_r(&seed) / (double)RAND_MAX - 1.0;
            double y = 2.0 * (double)rand_r(&seed) / (double)RAND_MAX - 1.0;

            if ((x * x + y * y) <= 1.0) {
                local_dentro++;
            }
        }

        #pragma omp critical
        {
            dentro_circulo += local_dentro;
        }
    }

    double pi = 4.0 * dentro_circulo / total_elementos;
    double erro = fabs(pi - M_PI);
    tempo = obter_tempo_segundos() - tempo;

    printf("PI = %.10f\nErro = %.10e\nTempo = %.9f segundos\n", pi, erro, tempo);

    return 0;
}
