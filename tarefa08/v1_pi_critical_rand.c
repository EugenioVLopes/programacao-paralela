/* v1: rand() + contador privado + critical (1x por thread)
 * Compilar: gcc -O2 -fopenmp -Wall -o pi_critical_rand pi_critical_rand.c -lm
 * Uso: OMP_NUM_THREADS=4 ./pi_critical_rand [total_elementos]
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

static double agora(void){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC,&t);
    return t.tv_sec + t.tv_nsec/1e9;
}

int main(int argc, char *argv[]){
    long total_elementos = argc>1 ? atol(argv[1]) : 5000000L;
    long total = 0;
    double tempo = agora();
    
    srand(12345);
    
    #pragma omp parallel shared(total,total_elementos) default(none)
    {
        long local = 0;

        #pragma omp for
        for(long i = 0; i < total_elementos; i++){
            double x = (double)rand()/(double)RAND_MAX * 2.0 - 1.0;
            double y = (double)rand()/(double)RAND_MAX * 2.0 - 1.0;
            
            if(x*x + y*y <= 1.0) {
                local++;
            }
        }

        #pragma omp critical
        total += local;
    }

    tempo = agora() - tempo;

    printf("v1 critical+rand: pi=%.6f tempo=%.4f s total=%ld\n", 4.0*total/total_elementos, tempo, total);
    
    return 0;
}
