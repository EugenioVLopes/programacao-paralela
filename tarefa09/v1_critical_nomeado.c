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
    No *head0 = NULL;
    No *head1 = NULL;
    double tempo = agora();
    #pragma omp parallel shared(head0, head1, total_elementos) default(none)
    {
        #pragma omp single
        {
            for (long i = 0; i < total_elementos; i++) {
                #pragma omp task firstprivate(i) shared(head0, head1) default(none)
                {
                    unsigned int seed = 12345u + (unsigned int)(i * 2654435761u);
                    int qual = (int)(rand_r(&seed) % 2);
                    int v = (int)(rand_r(&seed) % 1000000);
                    if (qual == 0) {
                        #pragma omp critical(lista0)
                        inserir(&head0, v);
                    } else {
                        #pragma omp critical(lista1)
                        inserir(&head1, v);
                    }
                }
            }
        }
    }
    tempo = agora() - tempo;
    long c0 = contar(head0);
    long c1 = contar(head1);
    printf("[v1 critical nomeado] N=%ld c0=%ld c1=%ld total=%ld %s tempo=%.4f s\n",
        total_elementos, c0, c1, c0 + c1, (c0 + c1 == total_elementos) ? "OK" : "ERRO", tempo);
    liberar(head0);
    liberar(head1);
    return 0;
}
