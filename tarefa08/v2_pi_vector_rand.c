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
    int total_threads = omp_get_max_threads();
    long *hits = calloc(total_threads, sizeof(long));
    
    srand(12345);
    
    double tempo = agora();
    
    #pragma omp parallel shared(hits,total_elementos) default(none)
    {
        int tid = omp_get_thread_num();
        
        #pragma omp for
        for(long i = 0; i < total_elementos; i++){
            double x = (double)rand()/(double)RAND_MAX * 2.0 - 1.0;
            double y = (double)rand()/(double)RAND_MAX * 2.0 - 1.0;
            
            if(x*x + y*y <= 1.0) {
                hits[tid]++;
            }
        }
    }
    
    long total = 0;
    
    for(int i = 0; i < total_threads; i++) {
        total += hits[i];
    }
    
    tempo = agora() - tempo;
    
    printf("v2 vetor+rand: pi=%.6f tempo=%.4f s total=%ld\n", 4.0*total/total_elementos, tempo, total);
    
    free(hits);    
    return 0;
}
