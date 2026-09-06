/* v3: rand_r() + contador privado + critical (1x por thread)
 * Cada thread tem seed privada: elimina o gargalo global do rand().
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
    long total_elementos = argc>1 ? atol(argv[1]) : 5000000L;
    long total = 0;
    double tempo = agora();
    
    #pragma omp parallel shared(total,total_elementos) default(none)
    {
        unsigned int seed = 12345u + 7919u*(unsigned int)omp_get_thread_num();
        long local = 0;
        
        #pragma omp for
        for(long i = 0; i < total_elementos; i++ ){
            double x = (double)rand_r(&seed)/(double)RAND_MAX * 2.0 - 1.0;
            double y = (double)rand_r(&seed)/(double)RAND_MAX * 2.0 - 1.0;
            
            if(x*x + y*y <= 1.0) {
                local++;
            }
        }
        
        #pragma omp critical
        total += local;
    }
    
    tempo = agora() - tempo;
    
    printf("v3 critical+rand_r: pi=%.6f tempo=%.4f s total=%ld\n", 4.0*total/total_elementos, tempo, total);
    
    return 0;
}
