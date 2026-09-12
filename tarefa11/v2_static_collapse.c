// gcc -std=c11 -O2 -Wall -Wextra -fopenmp v2_static_collapse.c -lm -o ns_v2
#ifndef _OPENMP
#error "Compile esta variante com -fopenmp"
#endif
#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

static int integer(const char *text, int minimum, int maximum, int *out) {
    char *end;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno || end == text || *end || value < minimum || value > maximum)
        return 0;
    *out = (int)value;
    return 1;
}

static double agora(void) {
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t)) {
        perror("clock_gettime");
        exit(EXIT_FAILURE);
    }
    return (double)t.tv_sec + (double)t.tv_nsec / 1e9;
}

int main(int argc, char **argv) {
    const int tamanho_grid = 512;
    const int num_passos_tempo = 500;
    int mode = 0, threads = 1;
    if (argc > 3 || (argc > 1 && !integer(argv[1], 0, 2, &mode))) {
        fprintf(stderr, "Uso: %s [modo=0|1|2 [campo.txt]]\n", argv[0]);
        return EXIT_FAILURE;
    }
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
    for (int i = 0; i < tamanho_grid; ++i) {
        for (int j = 0; j < tamanho_grid; ++j) {
            double x = i - (tamanho_grid - 1) / 2.0, y = j - (tamanho_grid - 1) / 2.0;
            double sigma = tamanho_grid / 10.0;
            int boundary = i == 0 || j == 0 || i == tamanho_grid-1 || j == tamanho_grid-1;
            u[(size_t)i*tamanho_grid+j] = mode == 2 ? 1.0 :
                (mode == 1 || boundary ? 0.0 :
                 0.1 * exp(-(x*x+y*y)/(2*sigma*sigma)));
        }
    }
    /* Bordas fixas iguais nos dois buffers: nenhuma escrita posterior. */
    memcpy(v, u, count * sizeof(*v));
    double start = agora();
    #pragma omp parallel default(none) shared(u,v,tamanho_grid,num_passos_tempo,r,threads)
    {
        #pragma omp single
        threads = omp_get_num_threads();
        for (int t = 0; t < num_passos_tempo; ++t) {
            #pragma omp for collapse(2) schedule(static)
            for (int i = 1; i < tamanho_grid-1; ++i) {
                for (int j = 1; j < tamanho_grid-1; ++j) {
                    size_t c = (size_t)i*tamanho_grid+j;
                    v[c] = u[c] + r * (u[c-1]+u[c+1]+u[c-tamanho_grid]+u[c+tamanho_grid]-4*u[c]);
                }
            } /* Barreira: todas as escritas terminam antes da troca. */
            #pragma omp single
            {
                double *tmp = u; u = v; v = tmp;
            } /* Barreira: todas as threads veem a troca antes do próximo passo. */
        }
    }
    double elapsed = agora() - start;
    double peak = u[0], minimum = u[0], sum = 0.0;
    for (size_t c = 0; c < count; ++c) {
        peak = fmax(peak, u[c]); minimum = fmin(minimum, u[c]); sum += u[c];
    }
    printf("%s tamanho_grid=%d num_passos_tempo=%d modo=%d threads=%d r=%.3f tempo=%.9f s minimo=%.17g pico=%.17g soma=%.17g\n",
           "v2_static_collapse", tamanho_grid, num_passos_tempo, mode, threads, r, elapsed, minimum, peak, sum);
    int status = EXIT_SUCCESS;
    if (argc == 3) {
        FILE *file = fopen(argv[2], "w");
        if (!file) { perror(argv[2]); status = EXIT_FAILURE; }
        else {
            for (size_t c = 0; c < count; ++c)
                if (fprintf(file, "%.17g%c", u[c], (c+1)%tamanho_grid ? ' ' : '\n') < 0)
                    status = EXIT_FAILURE;
            if (fclose(file)) status = EXIT_FAILURE;
        }
    }
    free(u); free(v);
    return status;
}
