#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct no {
    char nome_arquivo[128];
    struct no *next;
} No;

No *criar_no(const char *nome_arquivo) {
    No *novo_no = malloc(sizeof(No));
    if (!novo_no) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strncpy(novo_no->nome_arquivo, nome_arquivo, sizeof(novo_no->nome_arquivo) - 1);
    novo_no->nome_arquivo[sizeof(novo_no->nome_arquivo) - 1] = '\0';
    novo_no->next = NULL;
    return novo_no;
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
    No *atual = cabeca;
    while (atual != NULL) {
        No *temp = atual;
        atual = atual->next;
        free(temp);
    }
}

int main() {
    No *cabeca = NULL;

    inserir_no_fim(&cabeca, "documento1.txt");
    inserir_no_fim(&cabeca, "imagem2.png");
    inserir_no_fim(&cabeca, "dados3.csv");
    inserir_no_fim(&cabeca, "relatorio4.pdf");
    inserir_no_fim(&cabeca, "backup5.zip");

    No *atual = cabeca;
    while (atual != NULL) {
        printf("Arquivo: %s\n", atual->nome_arquivo);
        atual = atual->next;
    }

    liberar_lista(cabeca);
    return 0;
}
