#include <omp.h>
#include <stdio.h>

int main() {
    int x = 10;
    int y = 20;
    int soma = 0;

    #pragma omp parallel for default(none) shared(x, soma)
    for (int i = 0; i < 10; i++) {
        soma += x + y;  // 'y' nao esta classificada!
    }

    printf("soma = %d\n", soma);
    return 0;
}
