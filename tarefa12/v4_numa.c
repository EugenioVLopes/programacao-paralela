#define VERSION "v4_numa"
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#ifdef USE_PASCAL
#include <pascalops.h>
#endif

static double *evolve(double *u, double *v, int n, int steps, double r, int *threads);

static int integer(const char *text, int minimum, int *out) {
    char *end;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno || end == text || *end || value < minimum || value > INT_MAX)
        return 0;
    *out = (int)value;
    return 1;
}

int main(int argc, char **argv) {
    int n = 512, steps = 500, mode = 0, threads = 1;
    if (argc > 5 || (argc > 1 && !integer(argv[1], 3, &n)) ||
        (argc > 2 && !integer(argv[2], 0, &steps)) ||
        (argc > 3 && (!integer(argv[3], 0, &mode) || mode > 2))) {
        fprintf(stderr, "Uso: %s [N>=3 [passos>=0 [modo=0|1|2 [campo.bin]]]]\n", argv[0]);
        return EXIT_FAILURE;
    }
#ifdef WORKLOAD_INPUT
    /* PaScal: argumento e o alvo de celulas internas.
       Arredondar o lado permite dobrar a carga em malhas quadradas inteiras. */
    if (argc > 1) {
        int side = (int)round(sqrt((double)n));
        n = side + 2;
    }
#endif
    if ((size_t)n > SIZE_MAX / sizeof(double) / (size_t)n / 2) {
        fprintf(stderr, "Malha excede o espaco de enderecamento\n");
        return EXIT_FAILURE;
    }
    size_t count = (size_t)n * n;
    double total_start = omp_get_wtime();
    double *u = malloc(count * sizeof(*u)), *v = malloc(count * sizeof(*v));
    if (!u || !v) {
        fprintf(stderr, "Falha de alocacao para N=%d\n", n);
        free(u); free(v);
        return EXIT_FAILURE;
    }
    const double nu = 0.1, dt = 0.2, dx = 1.0;
    const double r = nu * dt / (dx * dx); /* 0.02 <= 1/4: FTCS estavel em 2D. */
    double init_start = omp_get_wtime();
#ifdef USE_PASCAL
    pascal_start(1); /* Inicializacao, inclui first touch. */
#endif
    #pragma omp parallel for schedule(static) default(none) shared(u,v,n,mode)
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double x = i - (n-1)/2.0, y = j - (n-1)/2.0;
            double sigma = n/10.0;
            int boundary = i == 0 || j == 0 || i == n-1 || j == n-1;
            size_t c = (size_t)i*n+j;
            u[c] = mode == 2 ? 1.0 : (mode == 1 || boundary ? 0.0 :
                   0.1 * exp(-(x*x+y*y)/(2*sigma*sigma)));
            v[c] = u[c]; /* Bordas fixas iguais nos dois buffers. */
        }
    }
#ifdef USE_PASCAL
    pascal_stop(1);
#endif
    double init_seconds = omp_get_wtime() - init_start;
#ifdef USE_PASCAL
    pascal_start(2); /* Evolucao completa; sem chamadas por celula/passo. */
#endif
    double start = omp_get_wtime();
    double *field = evolve(u, v, n, steps, r, &threads);
    double seconds = omp_get_wtime() - start;
#ifdef USE_PASCAL
    pascal_stop(2);
#endif
    double total_seconds = omp_get_wtime() - total_start;
    /* Diagnosticos e exportacao fora dos cronometros. */
    double minimum = field[0], peak = field[0], sum = 0.0;
    for (size_t c = 0; c < count; ++c) {
        if (!isfinite(field[c])) {
            fprintf(stderr, "Campo nao finito\n");
            free(u); free(v);
            return EXIT_FAILURE;
        }
        minimum = fmin(minimum, field[c]);
        peak = fmax(peak, field[c]);
        sum += field[c];
    }
    printf("%s,%d,%d,%d,%d,%.9f,%.9f,%.9f,%.17g,%.17g,%.17g\n",
           VERSION, n, steps, mode, threads, init_seconds, seconds, total_seconds,
           minimum, peak, sum);
    int status = EXIT_SUCCESS;
    if (argc == 5) {
        FILE *file = fopen(argv[4], "wb");
        if (!file) { perror(argv[4]); status = EXIT_FAILURE; }
        else {
            if (fwrite(field, sizeof(*field), count, file) != count) status = EXIT_FAILURE;
            if (fclose(file)) status = EXIT_FAILURE;
            if (status) fprintf(stderr, "Falha ao escrever campo\n");
        }
    }
    free(u); free(v);
    return status;
}

static double *evolve(double *u, double *v, int n, int steps, double r, int *threads) {
    #pragma omp parallel default(none) shared(u,v,n,steps,r,threads)
    {
        double *a = u, *b = v;
        #pragma omp single
        *threads = omp_get_num_threads();
        for (int t = 0; t < steps; ++t) {
            #pragma omp for schedule(static)
            for (int i = 1; i < n-1; ++i) {
                #pragma omp simd
                for (int j = 1; j < n-1; ++j) {
                    size_t c = (size_t)i*n+j;
                    b[c] = a[c] + r * (a[c-1]+a[c+1]+a[c-n]+a[c+n]-4*a[c]);
                }
            }
            double *tmp = a; a = b; b = tmp;
        }
    }
    return steps % 2 ? v : u;
}
