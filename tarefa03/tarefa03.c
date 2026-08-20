#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEFAULT_N 100000000L

static double agora(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int main(int argc, char **argv) {
    long n = argc > 1 ? atol(argv[1]) : DEFAULT_N;
    if (n <= 0) {
        fprintf(stderr, "Uso: %s [numero_de_elementos]\n", argv[0]);
        return 1;
    }

    int *v = malloc((size_t)n * sizeof(*v));
    if (!v) {
        perror("malloc");
        return 1;
    }

    double inicio = agora();
    for (long i = 0; i < n; ++i)
        v[i] = (int)(3 * i + 1);
    double tempo_inicializacao = agora() - inicio;

    inicio = agora();
    long long soma_dependente = 0;
    for (long i = 0; i < n; ++i)
        soma_dependente += v[i];
    double tempo_dependente = agora() - inicio;

    inicio = agora();
    long long s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    long i = 0;
    for (; i + 3 < n; i += 4) {
        s0 += v[i];
        s1 += v[i + 1];
        s2 += v[i + 2];
        s3 += v[i + 3];
    }
    for (; i < n; ++i)
        s0 += v[i];
    long long soma_multiplos = s0 + s1 + s2 + s3;
    double tempo_multiplos = agora() - inicio;

    printf("N=%ld inicializacao=%.9f dependente=%.9f multiplos=%.9f "
           "somas=%lld,%lld\n", n, tempo_inicializacao,
           tempo_dependente, tempo_multiplos, soma_dependente,
           soma_multiplos);
    free(v);
    return soma_dependente != soma_multiplos;
}
