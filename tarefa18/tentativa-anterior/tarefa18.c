#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef N
#define N 10000000
#endif
#define TOL 0.0000001f
//
// Soma de vetores c[i] = a[i] + b[i] (tutorial OpenMP em GPUs, UoB-HPC,
// adaptacao NPAD-UFRN). Compara no mesmo no e na mesma execucao:
//   - CPU (slide 27): "#pragma omp parallel for";
//   - GPU (exercicio 2): "#pragma omp target" + "#pragma omp loop".
// Vetores na pilha tem mapeamento implicito tofrom; escalares sao
// firstprivate. Exige "ulimit -s unlimited" (4 vetores x N floats na pilha).
// res usa a mesma aritmetica float de c para nao gerar falsos erros quando
// N > 2^24. Diagnostico vai para stderr; stdout recebe uma linha CSV:
// N,dispositivos,na_gpu,err_cpu,err_gpu,cpu_s,gpu_s
//
int main(void) {
    float a[N], b[N], c[N], res[N];
    int err_cpu = 0, err_gpu = 0;

    double t0 = omp_get_wtime();
#pragma omp parallel for
    for (long i = 0; i < N; i++) {
        a[i] = (float)i;
        b[i] = 2.0f * (float)i;
        c[i] = 0.0f;
        res[i] = 3.0f * (float)i;
    }
    double t_init = omp_get_wtime() - t0;

    t0 = omp_get_wtime();
#pragma omp parallel for
    for (long i = 0; i < N; i++)
        c[i] = a[i] + b[i];
    double t_cpu = omp_get_wtime() - t0;

#pragma omp parallel for reduction(+:err_cpu)
    for (long i = 0; i < N; i++) {
        float v = c[i] - res[i];
        if (v * v > TOL) err_cpu++;
    }

    // Sonda: 1 se a regiao target executou fora do host, 0 se houve fallback.
    int na_gpu = 0;
#pragma omp target map(tofrom: na_gpu)
    na_gpu = !omp_is_initial_device();

#pragma omp parallel for
    for (long i = 0; i < N; i++)
        c[i] = 0.0f;

    t0 = omp_get_wtime();
#pragma omp target
#pragma omp loop
    for (long i = 0; i < N; i++)
        c[i] = a[i] + b[i];
    double t_gpu = omp_get_wtime() - t0;

#pragma omp parallel for reduction(+:err_gpu)
    for (long i = 0; i < N; i++) {
        float v = c[i] - res[i];
        if (v * v > TOL) err_gpu++;
    }

    int dispositivos = omp_get_num_devices();
    fprintf(stderr, "N=%d dispositivos=%d na_gpu=%d err_cpu=%d err_gpu=%d\n",
            N, dispositivos, na_gpu, err_cpu, err_gpu);
    fprintf(stderr, "init: %.3fs cpu: %.6fs gpu: %.6fs\n", t_init, t_cpu, t_gpu);
    printf("%d,%d,%d,%d,%d,%.9f,%.9f\n",
           N, dispositivos, na_gpu, err_cpu, err_gpu, t_cpu, t_gpu);
    return (err_cpu || err_gpu) ? EXIT_FAILURE : EXIT_SUCCESS;
}
