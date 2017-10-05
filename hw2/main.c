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
    int threadNum = 3;
    #pragma omp parallel num_threads (threadNum) 
    {
        printf("hello world! \n");
        #pragma omp for schedule(static, 1)
        for(int i = 0; i < 4; ++i) {
            printf("Iteration %d, %d! \n", i, omp_get_thread_num());
        }
        printf("Finish! \n");
    }
}