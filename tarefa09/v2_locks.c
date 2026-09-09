#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

typedef struct No {
    int valor;
    struct No *prox;
} No;

static double agora(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

static void inserir(No **head, int valor) {
    No *novo = malloc(sizeof(No));
    novo->valor = valor;
    novo->prox = *head;
    *head = novo;
}

static long contar(No *head) {
    long n = 0;
    for (; head; head = head->prox) n++;
    return n;
}

static void liberar(No *head) {
    while (head) { No *t = head; head = head->prox; free(t); }
}

int main(int argc, char *argv[]) {
    long total_elementos = argc > 1 ? atol(argv[1]) : 50000L;
    int nlistas = argc > 2 ? atoi(argv[2]) : 8;
    if (nlistas < 1) nlistas = 1;
    No **heads = calloc((size_t)nlistas, sizeof(No *));
    omp_lock_t *locks = malloc((size_t)nlistas * sizeof(omp_lock_t));
    for (int i = 0; i < nlistas; i++) omp_init_lock(&locks[i]);
    double tempo = agora();
    #pragma omp parallel shared(heads, locks, total_elementos, nlistas) default(none)
    {
        #pragma omp single
        {
            for (long i = 0; i < total_elementos; i++) {
                #pragma omp task firstprivate(i) shared(heads, locks, nlistas) default(none)
                {
                    unsigned int seed = 999u + (unsigned int)(i * 40503u);
                    int qual = (int)(rand_r(&seed) % (unsigned int)nlistas);
                    int v = (int)(rand_r(&seed) % 1000000);
                    omp_set_lock(&locks[qual]);
                    inserir(&heads[qual], v);
                    omp_unset_lock(&locks[qual]);
                }
            }
        }
    }
    tempo = agora() - tempo;
    long total = 0;
    for (int i = 0; i < nlistas; i++) total += contar(heads[i]);
    printf("[v2 locks] N=%ld nlistas=%d total=%ld %s tempo=%.4f s\n",
        total_elementos, nlistas, total, (total == total_elementos) ? "OK" : "ERRO", tempo);
    for (int i = 0; i < nlistas; i++) {
        liberar(heads[i]);
        omp_destroy_lock(&locks[i]);
    }
    free(heads);
    free(locks);
    return 0;
}
