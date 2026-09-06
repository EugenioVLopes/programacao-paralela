/* v4 corrigida: padding de 64 bytes por posicao elimina false sharing.
 * Cada slot tem 64 bytes (linha de cache): hits ficam em linhas distintas.
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

typedef struct {
    long v;
    char pad[64-sizeof(long)]; 
} Slot __attribute__((aligned(64)));

static double agora(void){ 
    struct timespec t; clock_gettime(CLOCK_MONOTONIC,&t); 
    return t.tv_sec + t.tv_nsec/1e9; 
}

int main(int argc, char *argv[]){
    long total_elementos = argc>1 ? atol(argv[1]) : 5000000L;
    int P = omp_get_max_threads();
    
    Slot *hits = calloc(P, sizeof(Slot));

    double tempo = agora();
    
    #pragma omp parallel shared(hits,total_elementos) default(none)
    {
        unsigned int seed = 12345u + 7919u*(unsigned int)omp_get_thread_num();
        int tid = omp_get_thread_num();
        
        #pragma omp for
        for(long i=0;i<total_elementos;i++){
            double x = (double)rand_r(&seed)/ (double)RAND_MAX * 2.0 - 1.0;
            double y = (double)rand_r(&seed)/ (double)RAND_MAX * 2.0 - 1.0;
            
            if( x*x + y*y <= 1.0 ) hits[tid].v++;
        }
    }

    long total = 0;
    for(int i = 0; i <P; i++) total += hits[i].v;

    tempo = agora() - tempo;
    
    printf("v4-pad vetor+rand_r+pad64: pi=%.6f tempo=%.4f s total=%ld\n", 4.0*total/total_elementos, tempo, total);
    
    free(hits); return 0;
}
