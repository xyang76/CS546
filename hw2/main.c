#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

void Test(int n) {
    for(int i = 0; i < 10000; ++i) {
    //do nothing, just waste time
    }
    printf("%d, ", n);
}

int main(int argc,char* argv[]) {
    #pragma omp parallel for
    for ( int j = 0; j < 4; j++ )
    {
        printf("j = %d, ThreadId = %d\n", j, omp_get_thread_num());
    }
}