#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <numero_de_iteracoes>\n", argv[0]);
        return 1;
    }

    long long N = atoll(argv[1]);
    double pi = 0.0;

    for (long long k = 0; k < N; k++) {
        double termo = 1.0 / (2.0 * k + 1.0);
        pi += (k % 2 == 0) ? termo : -termo;
    }
    pi *= 4.0;

    double erro = fabs(pi - M_PI);

    printf("Iteracoes: %lld\n", N);
    printf("Pi:        %.15f\n", pi);
    printf("Erro:      %.2e\n", erro);

    return 0;
}
