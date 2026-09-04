#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>


typedef struct no {
    char nome_arquivo[128];
    struct no *next;
} No;

No *criar_no(const char *nome_arquivo) {
    No *no = malloc(sizeof(No));
    strncpy(no->nome_arquivo, nome_arquivo, sizeof(no->nome_arquivo) - 1);
    no->nome_arquivo[sizeof(no->nome_arquivo) - 1] = '\0';
    no->next = NULL;
    return no;
}

void inserir_no_fim(No **cabeca, const char *nome_arquivo) {
    No *novo_no = criar_no(nome_arquivo);
    if (*cabeca == NULL) {
        *cabeca = novo_no;
        return;
    }
    No *atual = *cabeca;
    while (atual->next != NULL) {
        atual = atual->next;
    }
    atual->next = novo_no;
}

void liberar_lista(No *cabeca) {
    while (cabeca) {
        No *temp = cabeca;
        cabeca = cabeca->next;
        free(temp);
    }
}

int main() {
    No *cabeca = NULL;
    inserir_no_fim(&cabeca, "doc1.txt");
    inserir_no_fim(&cabeca, "img2.png");
    inserir_no_fim(&cabeca, "dat3.csv");
    inserir_no_fim(&cabeca, "rep4.pdf");
    inserir_no_fim(&cabeca, "bkp5.zip");

    int nos_lista = 5;
    int processados = 0;

    #pragma omp parallel
    {
        #pragma omp single
        {
            No *atual = cabeca;
            while (atual != NULL) {
                #pragma omp task firstprivate(atual)
                {
                    printf("  [Thread %d] Arquivo: %s\n",
                           omp_get_thread_num(), atual->nome_arquivo);
                    processados++;
                }
                atual = atual->next;
            }
        }
    }

    printf("Nos na lista: %d | Processamentos contados: %d\n",
           nos_lista, processados);

    liberar_lista(cabeca);
    return 0;
}
