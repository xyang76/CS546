
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
#include <math.h>
#include <stdio.h>

typedef struct {float r; float i;} complex;
/* Main */
void read_file(char* path);
int main(int argc, char** argv) 
{
    float img[1024][1024];
    read_file("out");
}

void read_file(char* path) 
{
    FILE *f; /*open file descriptor */
    int i, j;
    f = fopen(path, "r");
    complex tmp;
    i = 0;
    while(1)
    {
        j = fscanf(f, "%6.2g", &tmp);
        if(j < 0) {
            break;
        }
        i++;
    }
    printf("i = %d\n", i);
    fclose(f);
}

