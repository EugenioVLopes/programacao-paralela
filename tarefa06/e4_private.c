#include <omp.h>
#include <stdio.h>

int main() {
    int x = 100;

    #pragma omp parallel for private(x)
    for (int i = 0; i < 8; i++) {
        printf("Thread %d: x (antes) = %d\n", omp_get_thread_num(), x);
        x = omp_get_thread_num();
        printf("Thread %d: x (depois) = %d\n", omp_get_thread_num(), x);
    }

    printf("Fora da regiao paralela: x = %d\n", x);
    return 0;
}
