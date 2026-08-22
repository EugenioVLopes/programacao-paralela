#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

#define DEFAULT_N 50000000L

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

    // Alocação para os testes Memory-bound (A, B, C)
    double *A = malloc((size_t)n * sizeof(*A));
    double *B = malloc((size_t)n * sizeof(*B));
    double *C = malloc((size_t)n * sizeof(*C));

    // Alocação para o teste CPU-bound (V)
    double *V = malloc((size_t)n * sizeof(*V));

    if (!A || !B || !C || !V) {
        perror("malloc");
        return 1;
    }

    // Inicialização
    for (long i = 0; i < n; ++i) {
        A[i] = i * 0.5;
        B[i] = i * 0.25;
        V[i] = i * 0.1;
    }

    int n_threads = omp_get_max_threads();

    // ----------------------------------------------------
    // TESTE 1: MEMORY-BOUND
    // Carga de trabalho trivial em termos de computação,
    // mas exige alta largura de banda da memória principal.
    // ----------------------------------------------------
    double inicio_mem = agora();
    
    #pragma omp parallel for
    for (long i = 0; i < n; ++i) {
        C[i] = A[i] + B[i];
    }
    
    double tempo_mem = agora() - inicio_mem;

    // ----------------------------------------------------
    // TESTE 2: CPU-BOUND
    // Pouco acesso à memória (apenas V[i] uma vez), mas
    // muitos ciclos de CPU executando cálculos matemáticos.
    // ----------------------------------------------------
    double inicio_cpu = agora();

    #pragma omp parallel for
    for (long i = 0; i < n; ++i) {
        double val = V[i];
        for (int j = 0; j < 50; ++j) {
            val = sqrt(val + j * 0.01) + sin(val);
        }
        V[i] = val;
    }

    double tempo_cpu = agora() - inicio_cpu;

    // Impressão dos resultados para o script parsear
    printf("N=%ld threads=%d memory_bound=%.9f cpu_bound=%.9f\n", 
           n, n_threads, tempo_mem, tempo_cpu);

    // Evita otimização excessiva que possa remover os cálculos
    double soma_check = C[n/2] + V[n/2];
    if (soma_check == 0.0) {
        printf(" \n"); // apenas para usar a variável e enganar o otimizador
    }

    free(A); free(B); free(C); free(V);

    return 0;
}
