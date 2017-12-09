
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

#define SIZE 512
int proc_num, proc_rank, i, j;              // Global variables: proc_num and proc_rank.
MPI_Datatype FFT_COMPLEX;                   // FFT_COMPLEX;
MPI_Datatype row_type, col_type;            // Row-type and col-type
double cal_start, cal_end;
double com_start, com_end;               
double cal_sum = 0, com_sum = 0;   
int cal = 0, com = 0; 

/* Methods declaration */
#define C_SWAP(a,b) {ctmp=(a);(a)=(b);(b)=ctmp;}
void c_fft1d(complex *r, int n, int isign);
void fft_2d_RB(complex img[][SIZE], int isign);
void MM_Point_RB(complex img1[][SIZE], complex img2[][SIZE], complex img3[][SIZE]);
void read_file(char* path, complex img[][SIZE]);
void write_file(char* path, complex img[][SIZE]);
void calculate();
void communicate();
void print(complex tmp[][SIZE]);

/* Main */
int main(int argc, char** argv) 
{
    complex img_1[SIZE][SIZE], img_2[SIZE][SIZE], img_3[SIZE][SIZE];
    MPI_Datatype col;
    // Initialization and get proc_num and proc_rank.
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &proc_num);
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_rank);
    
    // MPI_Type define
    MPI_Type_contiguous(2, MPI_FLOAT, &FFT_COMPLEX);
    MPI_Type_commit(&FFT_COMPLEX);
    MPI_Type_vector(SIZE, 1, SIZE, FFT_COMPLEX, &col);
    MPI_Type_commit(&col);
    MPI_Type_create_resized(col, 0, 1*sizeof(complex), &col_type);
    MPI_Type_commit(&col_type);
    MPI_Type_contiguous(SIZE, FFT_COMPLEX, &row_type);
    MPI_Type_commit(&row_type);
    
    if(proc_rank == 0) {
        read_file("im1", img_1);
        read_file("im2", img_2);
    }
    fft_2d_RB(img_1, -1);
    fft_2d_RB(img_2, -1);
    MM_Point_RB(img_1, img_2, img_3);
//    print(out);
    MPI_Finalize();
}

void fft_2d_RB(complex img[][SIZE], int isign)
{
    complex tmp[SIZE][SIZE];
    int chunk_size = SIZE / proc_num;
    MPI_Scatter(&img[0][0], chunk_size, row_type, &tmp[0][0], chunk_size, row_type, 0, MPI_COMM_WORLD);
    for(i = 0; i < chunk_size; i++) {
        c_fft1d(tmp[i], SIZE, isign);
    }
    MPI_Gather(&tmp[0][0], chunk_size, row_type, &img[0][0], chunk_size, row_type, 0, MPI_COMM_WORLD);
    
    MPI_Scatter(&img[0][0], chunk_size, col_type, &tmp[0][0], chunk_size, row_type, 0, MPI_COMM_WORLD);
    for(i = 0; i < chunk_size; i++) {
        c_fft1d(tmp[i], SIZE, isign);
    }
    MPI_Gather(&tmp[0][0], chunk_size, row_type, &img[0][0], chunk_size, col_type, 0, MPI_COMM_WORLD);
}

void MM_Point_RB(complex img1[][SIZE], complex img2[][SIZE], complex img3[][SIZE])
{
    printf("01\n");
}
//{
////    int chunk_size = SIZE / proc_num;
////    complex tmp1[SIZE][SIZE], tmp2[SIZE][SIZE], tmp0[SIZE][SIZE];
//    printf("01\n");
//    // Scatter with row.
////    MPI_Scatter(&img1[0][0], chunk_size, row_type, &tmp1[0][0], chunk_size, row_type, 0, MPI_COMM_WORLD);
////    MPI_Scatter(&img2[0][0], chunk_size, row_type, &tmp2[0][0], chunk_size, row_type, 0, MPI_COMM_WORLD);
//    printf("02\n");
////    for(i = 0; i < chunk_size; i++) {
////        for(j = 0; j < SIZE; j++) {
////            tmp0[i][j].r = tmp1[i][j].r * tmp2[i][j].r - tmp1[i][j].i * tmp2[i][j].i;
////            tmp0[i][j].i = tmp1[i][j].i * tmp2[i][j].r + tmp1[i][j].r * tmp2[i][j].i;
////        }
////    }
//    printf("03\n");
////    MPI_Gather(&tmp0[0][0], chunk_size, row_type, &out[0][0], chunk_size, row_type, 0, MPI_COMM_WORLD);
//    printf("04\n");
//}

void read_file(char* path, complex img[][SIZE]) 
{
    FILE *f; 
    f = fopen(path, "r");
    for (i=0; i<SIZE; i++)
    {
        for (j=0;j<SIZE;j++)
        {
            fscanf(f, "%g", &img[i][j].r); 
        }
    }
    fclose(f);
}

void c_fft1d(complex *r, int n, int isign)
{
   int     m,i,i1,j,k,i2,l,l1,l2;
   float   c1,c2,z;
   complex t, u;

   if (isign == 0) return;

   /* Do the bit reversal */
   i2 = n >> 1;
   j = 0;
   for (i=0;i<n-1;i++) {
      if (i < j)
         C_SWAP(r[i], r[j]);
      k = i2;
      while (k <= j) {
         j -= k;
         k >>= 1;
      }
      j += k;
   }

   /* m = (int) log2((double)n); */
   for (i=n,m=0; i>1; m++,i/=2);

   /* Compute the FFT */
   c1 = -1.0;
   c2 =  0.0;
   l2 =  1;
   for (l=0;l<m;l++) {
      l1   = l2;
      l2 <<= 1;
      u.r = 1.0;
      u.i = 0.0;
      for (j=0;j<l1;j++) {
         for (i=j;i<n;i+=l2) {
            i1 = i + l1;

            /* t = u * r[i1] */
            t.r = u.r * r[i1].r - u.i * r[i1].i;
            t.i = u.r * r[i1].i + u.i * r[i1].r;

            /* r[i1] = r[i] - t */
            r[i1].r = r[i].r - t.r;
            r[i1].i = r[i].i - t.i;

            /* r[i] = r[i] + t */
            r[i].r += t.r;
            r[i].i += t.i;
         }
         z =  u.r * c1 - u.i * c2;

         u.i = u.r * c2 + u.i * c1;
         u.r = z;
      }
      c2 = sqrt((1.0 - c1) / 2.0);
      if (isign == -1) /* FWD FFT */
         c2 = -c2;
      c1 = sqrt((1.0 + c1) / 2.0);
   }

   /* Scaling for inverse transform */
   if (isign == 1) {       /* IFFT*/
      for (i=0;i<n;i++) {
         r[i].r /= n;
         r[i].i /= n;
      }
   }
}

void calculate()
{
    if(cal == 0) {
        cal = 1;
        cal_start = MPI_Wtime();
    } else {
        cal = 0;
        cal_end = MPI_Wtime();
        cal_sum += cal_end - cal_start;
    }
}

void communicate()
{
    if(com == 0) {
        com = 1;
        com_start = MPI_Wtime();
    } else {
        com = 0;
        com_end = MPI_Wtime();
        com_sum += com_end - com_start;
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