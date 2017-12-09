
/*
 ------------------------------------------------------------------------
 FFT1D            c_fft1d(r,i,-1)
 Inverse FFT1D    c_fft1d(r,i,+1)
 ------------------------------------------------------------------------
*/
/* ---------- FFT 1D
   This computes an in-place complex-to-complex FFT
   r is the real and imaginary arrays of n=2^m points.
   isign = -1 gives forward transform
   isign =  1 gives inverse transform
*/

#include <assert.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>

typedef struct {float r; float i;} complex;
static complex ctmp;

#define SIZE 8
int proc_num, proc_rank, i, j;              // Global variables: proc_num and proc_rank.
MPI_Datatype FFT_COMPLEX;                   // FFT_COMPLEX;
MPI_Datatype row_type, col_type;            // Row-type and col-type

void print(complex tmp[][SIZE]);
void test(complex img[][SIZE]);

/* Main */
int main(int argc, char** argv) 
{
    complex img[SIZE][SIZE];
    MPI_Datatype col;
    // Initialization and get proc_num and proc_rank.
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    
    MPI_Type_contiguous(2, MPI_FLOAT, &FFT_COMPLEX);
    MPI_Type_commit(&FFT_COMPLEX);
    MPI_Type_vector(SIZE, 1, SIZE, FFT_COMPLEX, &col);
    MPI_Type_commit(&col);
    MPI_Type_create_resized(col, 0, 1*sizeof(complex), &col_type);
    MPI_Type_commit(&col_type);
    MPI_Type_contiguous(SIZE, FFT_COMPLEX, &row_type);
    MPI_Type_commit(&row_type);
    
    if(proc_rank == 0) {
        for(i = 0; i < SIZE; i++) {
            for(j = 0; j < SIZE; j++) {
                img[i][j].r = i*SIZE + j;
                img[i][j].i = 0;
            }
        }
    }
    test(img);
    
    
    
    MPI_Finalize();
}

void test(complex img[][SIZE])
{
    complex tmp[SIZE][SIZE];
    for(i = 0; i < SIZE; i++) {
        for(j = 0; j < SIZE; j++) {
            tmp[i][j] = 0;
        }
    }
    MPI_Scatter(&img[0][0], 2, col_type, &tmp[0][0], 2, row_type, 0, MPI_COMM_WORLD);
   
    for(i = 0; i < proc_num; i++) {
        if(proc_rank == i) {
            printf("Proc %d------\n", proc_rank);
            print(tmp);
        }
    }
}

void print(complex tmp[][SIZE])
{
    for(i = 0; i < SIZE; i++) {
        for(j = 0; j < SIZE; j++) {
            printf("%d,%d=%f\t", i, j, tmp[i][j].r);
        }
        printf("\n");
    }
}