// gcc -std=c11 -O2 -Wall -Wextra -fopenmp v2_static_collapse.c -lm -o ns_v2
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void inicializar_campos(double *campo_atual, double *proximo_campo, int tamanho_grid,
                               int modo) {
    size_t total = (size_t)tamanho_grid * tamanho_grid;
    double sigma = tamanho_grid / 10.0;
    for (int i = 0; i < tamanho_grid; ++i)
        for (int j = 0; j < tamanho_grid; ++j) {
            double x = i - (tamanho_grid - 1) / 2.0, y = j - (tamanho_grid - 1) / 2.0;
            int borda = i == 0 || j == 0 || i == tamanho_grid - 1 || j == tamanho_grid - 1;
            size_t indice = (size_t)i * tamanho_grid + j;
            campo_atual[indice] =
                modo == 2
                    ? 1.0
                    : (modo == 1 || borda ? 0.0
                                          : 0.1 * exp(-(x * x + y * y) / (2.0 * sigma * sigma)));
        }
    memcpy(proximo_campo, campo_atual, total * sizeof(*proximo_campo));
}

static int escrever_campo(const char *nome, const double *campo, int tamanho_grid) {
    size_t total = (size_t)tamanho_grid * tamanho_grid;
    FILE *arquivo = fopen(nome, "w");
    if (!arquivo) {
        perror(nome);
        return EXIT_FAILURE;
    }
    for (size_t indice = 0; indice < total; ++indice)
        if (fprintf(arquivo, "%.17g%c", campo[indice], (indice + 1) % tamanho_grid ? ' ' : '\n') <
            0) {
            fclose(arquivo);
            return EXIT_FAILURE;
        }
    return fclose(arquivo) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main(int argc, char **argv) {
    const int tamanho_grid = 512;
    const int num_passos_tempo = 500;

    if (argc > 2) {
        fprintf(stderr, "Uso: %s [campo.txt]\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    int threads = 1;
    size_t count = (size_t)tamanho_grid * tamanho_grid;
    
    double *u = malloc(count * sizeof(*u));
    double *v = malloc(count * sizeof(*v));
    
    if (!u || !v) {
        fprintf(stderr, "Falha de alocacao\n");
        free(u); free(v);
        return EXIT_FAILURE;
    }
    
    const double nu = 0.1, dt = 0.2, dx = 1.0;
    const double r = nu * dt / (dx * dx); /* 0.02 <= 1/4 em 2D */
    
    inicializar_campos(u, v, tamanho_grid, 0);
    
    double start = omp_get_wtime();
    
    #pragma omp parallel default(none) shared(u, v, tamanho_grid, num_passos_tempo, r, threads)
    {
        #pragma omp single
        threads = omp_get_num_threads();

        for (int t = 0; t < num_passos_tempo; ++t) {
            #pragma omp for collapse(2) schedule(static)
            for (int i = 1; i < tamanho_grid - 1; ++i) {
                for (int j = 1; j < tamanho_grid - 1; ++j) {
                    size_t indice = (size_t)i * tamanho_grid + j;
                    v[indice] =
                        u[indice] + r * (u[indice - 1] + u[indice + 1] + u[indice - tamanho_grid] +
                                         u[indice + tamanho_grid] - 4 * u[indice]);
                }
            } /* Barreira: todas as escritas terminam antes da troca. */

            #pragma omp single
            {
                double *temporario = u;
                u = v;
                v = temporario;
            } /* Barreira: todas as threads veem a troca antes do próximo passo. */
        }
    }
    
    double elapsed = omp_get_wtime() - start;
    
    double peak = u[0], minimum = u[0], sum = 0.0;
    
    for (size_t c = 0; c < count; ++c) {
        peak = fmax(peak, u[c]);
        minimum = fmin(minimum, u[c]);
        sum += u[c];
    }

    printf("%s tamanho_grid=%d num_passos_tempo=%d threads=%d r=%.3f tempo=%.9f "
           "s minimo=%.17g pico=%.17g soma=%.17g\n",
           "v2_static_collapse", tamanho_grid, num_passos_tempo, threads, r, elapsed, minimum, peak,
           sum);

    int status = EXIT_SUCCESS;
    if (argc == 2)
        status = escrever_campo(argv[1], u, tamanho_grid);
    free(u);
    free(v);
    return status;
}
