#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_ELEMENTOS_PADRAO 100000000L

static double obter_tempo_segundos(void) {
    struct timespec tempo_atual;
    clock_gettime(CLOCK_MONOTONIC, &tempo_atual);
    return (double)tempo_atual.tv_sec + (double)tempo_atual.tv_nsec / 1e9;
}

int main(int argc, char **argv) {
    long total_elementos = argc > 1 ? atol(argv[1]) : TOTAL_ELEMENTOS_PADRAO;
    if (total_elementos <= 0) {
        fprintf(stderr, "Uso: %s [numero_de_elementos]\n", argv[0]);
        return 1;
    }

    int *v = malloc((size_t)total_elementos * sizeof(*v));
    if (!v) {
        perror("malloc");
        return 1;
    }

    double tempo_inicio = obter_tempo_segundos();
    for (long i = 0; i < total_elementos; ++i)
        v[i] = (int)(3 * i + 1);
    double tempo_inicializacao = obter_tempo_segundos() - tempo_inicio;

    tempo_inicio = obter_tempo_segundos();
    long long soma_dependente = 0;
    for (long i = 0; i < total_elementos; ++i)
        soma_dependente += v[i];
    double tempo_dependente = obter_tempo_segundos() - tempo_inicio;

    tempo_inicio = obter_tempo_segundos();
    long long s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    long i = 0;
    for (; i + 3 < total_elementos; i += 4) {
        s0 += v[i];
        s1 += v[i + 1];
        s2 += v[i + 2];
        s3 += v[i + 3];
    }
    for (; i < total_elementos; ++i)
        s0 += v[i];
    long long soma_multiplos = s0 + s1 + s2 + s3;
    double tempo_multiplos = obter_tempo_segundos() - tempo_inicio;

    printf("N=%ld inicializacao=%.9f dependente=%.9f multiplos=%.9f "
           "somas=%lld,%lld\n", total_elementos, tempo_inicializacao,
           tempo_dependente, tempo_multiplos, soma_dependente,
           soma_multiplos);
    free(v);
    return soma_dependente != soma_multiplos;
}
