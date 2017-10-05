#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _OPENMP
    fprintf(stderr, "OpenMP not supported");
#endif

void Test(int n) {
    for(int i = 0; i < 10000; ++i) {
    //do nothing, just waste time
    }
    printf("%d, ", n);
}

int main(int argc,char* argv[]) {
    #pragma omp parallel num_threads (3) 
    {
        printf("hello world! \n");
        #pragma omp for
        for(int i = 0; i < 4; ++i) {
            printf("Iteration %d, %d! \n", i, omp_get_thread_num());
        }
        printf("Finish! \n");
    }
}