// Gerador da demonstração; os sete programas de benchmark continuam independentes.
// gcc -std=c11 -O2 -Wall -Wextra gerar_quadros.c -lm -o /tmp/gerar_quadros
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { GRID = 512, SAMPLES = 65, FRAMES = 201, STEPS = 2100000 };

// Espaçamento quadrático: mais quadros no início, com arredondamento inteiro.
static int frame_step(int frame) {
    const int intervals_squared = (FRAMES - 1) * (FRAMES - 1);
    return ((long long)STEPS * frame * frame + intervals_squared / 2) / intervals_squared;
}

// Arquivo float32 little-endian, inclusive em máquinas com outra ordem de bytes.
static void write_float(float value) {
    uint32_t bits;
    memcpy(&bits, &value, sizeof(bits));
    unsigned char bytes[4] = {bits, bits >> 8, bits >> 16, bits >> 24};
    if (fwrite(bytes, sizeof(bytes), 1, stdout) != 1) {
        perror("Escrita dos quadros");
        exit(EXIT_FAILURE);
    }
}

static void snapshot(const double *u) {
    double peak = 0;
    for (size_t c = 0; c < (size_t)GRID * GRID; ++c)
        peak = fmax(peak, u[c]);
    write_float((float)peak);
    for (int i = 0; i < SAMPLES; ++i) {
        int row = (int)round((double)i * (GRID - 1) / (SAMPLES - 1));
        for (int j = 0; j < SAMPLES; ++j) {
            int col = (int)round((double)j * (GRID - 1) / (SAMPLES - 1));
            write_float((float)u[(size_t)row * GRID + col]);
        }
    }
}

int main(void) {
    _Static_assert(sizeof(float) == 4 && FLT_RADIX == 2 && FLT_MANT_DIG == 24 &&
                   FLT_MAX_EXP == 128, "Requer float IEEE 754 binary32");
    size_t count = (size_t)GRID * GRID;
    double *u = calloc(count, sizeof(*u)), *v = calloc(count, sizeof(*v));
    if (!u || !v) {
        fprintf(stderr, "Falha de alocacao\n");
        free(u); free(v);
        return EXIT_FAILURE;
    }
    const double nu = 0.1, dt = 0.2, dx = 1.0, sigma = GRID / 10.0;
    const double r = nu * dt / (dx * dx);
    for (int i = 1; i < GRID - 1; ++i) {
        for (int j = 1; j < GRID - 1; ++j) {
            double x = i - (GRID - 1) / 2.0, y = j - (GRID - 1) / 2.0;
            u[(size_t)i * GRID + j] = 0.1 * exp(-(x*x + y*y) / (2*sigma*sigma));
        }
    }
    memcpy(v, u, count * sizeof(*v));
    // Cabeçalho: grade, amostras por eixo, passos totais, quadros, dt.
    write_float(GRID); write_float(SAMPLES); write_float(STEPS);
    write_float(FRAMES); write_float((float)dt);
    snapshot(u);
    int frame = 1;
    for (int t = 1; t <= STEPS; ++t) {
        for (int i = 1; i < GRID - 1; ++i) {
            for (int j = 1; j < GRID - 1; ++j) {
                size_t c = (size_t)i * GRID + j;
                v[c] = u[c] + r * (u[c-1]+u[c+1]+u[c-GRID]+u[c+GRID]-4*u[c]);
            }
        }
        double *tmp = u; u = v; v = tmp;
        if (t == frame_step(frame)) {
            snapshot(u);
            if (frame % 50 == 0) fprintf(stderr, "Quadro %d/%d: passo %d\n", frame + 1, FRAMES, t);
            ++frame;
        }
    }
    free(u); free(v);
    if (fflush(stdout)) { perror("Escrita dos quadros"); return EXIT_FAILURE; }
    return EXIT_SUCCESS;
}
