#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <time.h>
#include <omp.h>
#define N 10000000
#define TOL  0.0000001
//
//  This is a simple program to add two vectors
//  and verify the results.
//
//  History: Written by Tim Mattson, November 2017
//
static double agora(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec * 1e-9;
}

int main(void)
{

    float a[N], b[N], c[N], res[N];
    int err=0;

    double init_time, compute_time, test_time;
    init_time    = -agora();

   // fill the arrays
   #pragma omp parallel for
   for (int i=0; i<N; i++){
      a[i] = (float)i;
      b[i] = 2.0*(float)i;
      c[i] = 0.0;
      res[i] = i + 2*i;
   }

   init_time    +=  agora();
   compute_time  = -agora();
   
   // add two vectors
   #pragma omp parallel for
   for (int i=0; i<N; i++){
      c[i] = a[i] + b[i];
   }

   compute_time +=  agora();
   test_time     = -agora();

   // test results
   #pragma omp parallel for reduction(+:err)
   for(int i=0;i<N;i++){
      float val = c[i] - res[i];
      val = val*val;
      if(val>TOL) err++;
   }

   test_time    +=  agora();
   
   printf(" vectors added with %d errors\n",err);

   printf("Init time:    %.6fs\n", init_time);
   printf("Compute time: %.6fs\n", compute_time);
   printf("Test time:    %.6fs\n", test_time);
   printf("Total time:   %.6fs\n", init_time + compute_time + test_time);
   return err != 0;
}
