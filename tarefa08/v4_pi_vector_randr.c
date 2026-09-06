/* v4: rand_r() + vetor compartilhado hits[tid]++ (false sharing visivel)
 * Sem o lock do rand(), o ping-pong MESI do vetor aparece no tempo.
 */
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
static double agora(void){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC,&t); 
    return t.tv_sec + t.tv_nsec/1e9; 
}

int main(int argc, char *argv[]){
    long numero_elementos = argc>1 ? atol(argv[1]) : 5000000L;
    int P = omp_get_max_threads();

    long *hits = calloc(P, sizeof(long));

    double tempo = agora();
    
    #pragma omp parallel shared(hits,numero_elementos) default(none)
    {
        unsigned int seed = 12345u + 7919u*(unsigned int)omp_get_thread_num();
        
        int tid = omp_get_thread_num();

        #pragma omp for
        for(long i=0;i<numero_elementos;i++){
            double x = (double)rand_r(&seed)/(double)RAND_MAX*2.0-1.0;
            double y = (double)rand_r(&seed)/(double)RAND_MAX*2.0-1.0;
            if(x*x+y*y<=1.0) hits[tid]++;
        }
    }
    long total=0; for(int i=0;i<P;i++) total+=hits[i];
    
    tempo = agora() - tempo;

    printf("v4 vetor+rand_r: pi=%.6f tempo=%.4f s total=%ld\n", 4.0*total/numero_elementos, tempo, total);
    
    free(hits); 
    
    return 0;
}
