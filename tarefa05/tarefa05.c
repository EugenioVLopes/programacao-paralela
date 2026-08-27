#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define TOTAL_ELEMENTOS_PADRAO 5000000L

static double obter_tempo_segundos(void) {
    struct timespec tempo_atual;
    clock_gettime(CLOCK_MONOTONIC, &tempo_atual);
    return (double)tempo_atual.tv_sec + (double)tempo_atual.tv_nsec / 1e9;
}

int eh_primo(long numero) {
    if (numero <= 1) return 0;
    if (numero == 2) return 1;
    if (numero % 2 == 0) return 0;

    for (long divisor = 3; divisor * divisor <= numero; divisor += 2) {
        if (numero % divisor == 0) return 0;
    }

    return 1;
}

int main(int argc, char **argv) {
    long total_elementos = argc > 1 ? atol(argv[1]) : TOTAL_ELEMENTOS_PADRAO;
    
    if (total_elementos <= 0) {
        fprintf(stderr, "Uso: %s [total_elementos]\n", argv[0]);
        return 1;
    }

    int total_threads = omp_get_max_threads();

    // SEQUENCIAL
    double tempo_sequencial = obter_tempo_segundos();
    long total_primos_sequencial = 0;

    for (long i = 2; i <= total_elementos; ++i) {
        if (eh_primo(i)) {
            total_primos_sequencial++;
        }
    }
    tempo_sequencial = obter_tempo_segundos() - tempo_sequencial;

    // PARALELA
    double tempo_paralelo = obter_tempo_segundos();
    long total_primos_paralelo = 0;

    #pragma omp parallel for
    for (long i = 2; i <= total_elementos; ++i) {
        if (eh_primo(i)) {
            total_primos_paralelo++;
        }
    }
    tempo_paralelo = obter_tempo_segundos() - tempo_paralelo;

    printf("N=%ld threads=%d seq_time=%.9f par_time=%.9f seq_count=%ld par_count=%ld\n",
           total_elementos, total_threads,
           tempo_sequencial, tempo_paralelo,
           total_primos_sequencial, total_primos_paralelo);

    return 0;
}
