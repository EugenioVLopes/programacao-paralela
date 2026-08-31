#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define TOTAL_ELEMENTOS_PADRAO 5000000L

int main(int argc, char *argv[]) {
    long long total_elementos = argc > 1 ? atoll(argv[1]) : TOTAL_ELEMENTOS_PADRAO;

    long long dentro_circulo = 0;

    unsigned int seed = time(NULL);

    for (long long i = 0; i < total_elementos; i++) {
        double x = 2.0 * (double)rand_r(&seed) / (double)RAND_MAX - 1.0;
        double y = 2.0 * (double)rand_r(&seed) / (double)RAND_MAX - 1.0;

        if ((x * x + y * y) <= 1.0) {
            dentro_circulo++;
        }
    }

    double pi = 4.0 * dentro_circulo / total_elementos;
    double erro = fabs(pi - M_PI);

    printf("Dentro do circulo = %lld\nPI = %.10f\nErro = %.10e\n", dentro_circulo, pi, erro);

    return 0;
}