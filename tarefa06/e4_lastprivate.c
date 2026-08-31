#include <omp.h>
#include <stdio.h>

int main() {
    int resultado = 0;

    #pragma omp parallel for lastprivate(resultado)
    for (int i = 0; i < 20; i++) {
        resultado = i * i;
        printf("Thread %d: i = %d, resultado = %d\n", omp_get_thread_num(), i, resultado);
    }

    printf("Apos o laco: resultado = %d (esperado: %d)\n", resultado, 19 * 19);
    return 0;
}
