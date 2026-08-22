#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define DEFAULT_N 5000000L

static double agora(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

int is_prime(long n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (long i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(int argc, char **argv) {
    long n = argc > 1 ? atol(argv[1]) : DEFAULT_N;
    if (n <= 0) {
        fprintf(stderr, "Uso: %s [numero_de_elementos]\n", argv[0]);
        return 1;
    }

    int n_threads = omp_get_max_threads();

    // TESTE 1: SEQUENCIAL
    double inicio_seq = agora();
    long count_seq = 0;
    
    for (long i = 2; i <= n; ++i) {
        if (is_prime(i)) {
            count_seq++;
        }
    }
    double tempo_seq = agora() - inicio_seq;

    // TESTE 2: OPENMP NAIVE (RACE CONDITION)
    double inicio_omp_naive = agora();
    long count_omp_naive = 0;

    #pragma omp parallel for
    for (long i = 2; i <= n; ++i) {
        if (is_prime(i)) {
            count_omp_naive++;
        }
    }
    double tempo_omp_naive = agora() - inicio_omp_naive;

    // TESTE 3: OPENMP REDUCTION (CORRETO)
    double inicio_omp_red = agora();
    long count_omp_red = 0;

    #pragma omp parallel for reduction(+:count_omp_red)
    for (long i = 2; i <= n; ++i) {
        if (is_prime(i)) {
            count_omp_red++;
        }
    }
    double tempo_omp_red = agora() - inicio_omp_red;

    printf("N=%ld threads=%d seq_time=%.9f omp_naive_time=%.9f omp_red_time=%.9f seq_count=%ld omp_naive_count=%ld omp_red_count=%ld\n", 
           n, n_threads, tempo_seq, tempo_omp_naive, tempo_omp_red, count_seq, count_omp_naive, count_omp_red);

    return 0;
}
