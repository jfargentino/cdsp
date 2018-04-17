#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"

/**************************************************************************

MATTST.C - TEST MATRIX FUNCTIONS

INPUTS: NONE

OUTPUTS: PRINT OF TEST MATRICES AND RESULTS

TESTS THE FOLLOWING FUNCTIONS:

    matrix_allocate    allocate matrix, return pointer to MATRIX structure
    matrix_free        free matrix area and MATRIX structure
    matrix_print       print the elements of a matrix to stdout
    matrix_scale       scale all elements in the matrix by s
    matrix_transpose   transpose a matrix, return pointer to new MATRIX
    matrix_sub         subtract two matrices and return a difference MATRIX
    matrix_mult        multiply two matrices, return new MATRIX
    matrix_invert      invert a square matrix, return inverse MATRIX
    matrix_det         return the determinant of a square matrix

*************************************************************************/

main()
{
    MATRIX *A,*Ai,*At,*B,*Bt,*H,*Hi,*Hie;
    double **h,**hie;
    short **b,**bt,**a,**at;
    short *atemp;
    int i,j,n;
    long int diag,rest;
    double denom;

/* integer 3x3 matrix test */
    A = matrix_allocate(3,3,sizeof(short));

    a = (short **)A->ptr;
    a[0][0]=-1;    a[0][1]=2;    a[0][2]=1;
    a[1][0]=-2;    a[1][1]=2;    a[1][2]=1;
    a[2][0]=-1;    a[2][1]=2;    a[2][2]=3;

    printf("\nA matrix:");
    matrix_print(A);

    Ai = matrix_invert(A);

    printf("\n\nA matrix inverse (Ai):");
    matrix_print(Ai);

    printf("\n\nProduct of A and A inverse (should be I):");
    matrix_print(matrix_mult(A,Ai));

    B = matrix_allocate(3,1,sizeof(short));
    b = (short **)B->ptr;

    b[0][0] = 5;
    b[1][0] = 6;
    b[2][0] = 9;

    printf("\n\nb vector:");
    matrix_print(B);

    printf("\n\nx result (x=Ai*b):");
    matrix_print(matrix_mult(Ai,B));

    matrix_free(Ai);

/* Crammer's rule test of determinant and transpose */
    At = matrix_transpose(A);
    at = (short **)At->ptr;
    Bt = matrix_transpose(B);
    bt = (short **)Bt->ptr;

    denom = matrix_det(At);
    printf("\n\nCrammer's rule denominator = %lf",denom);

    for(i = 0 ; i < 3 ; i++) {
        atemp = at[i];
        at[i] = bt[0];
        printf("\n\nNumerator matrix transpose for x[%d]:",i);
        matrix_print(At);
        printf("\nCrammer's rule x[%d] = %14.10lf",i,matrix_det(At)/denom);
        at[i] = atemp;
    }

    matrix_free(At);
    matrix_free(B);
    matrix_free(Bt);

/* invert and find the determinant of Hilbert matrices 2 to 6 */

    for(n = 2 ; n < 7 ; n++) {

        H = matrix_allocate(n,n,sizeof(double));

        h = (double **)H->ptr;

        for(i = 0 ; i < n ; i++)
            for(j = 0 ; j < n ; j++)
                h[i][j] = 1.0/(double)(i+j+1);

        printf("\n\nHilbert matrix N=%d",i);
        matrix_print(H);

        printf("\nHilbert determinant = %17.10le",matrix_det(H));

        Hi = matrix_invert(H);

        printf("\n\nCalculated Hilbert matrix inverse N=%d",i);
        matrix_print(Hi);

/* make exact Hilbert inverse matrix */
        Hie = matrix_allocate(n,n,sizeof(double));
        hie = (double **)Hie->ptr;

        for(i = 0 ; i < n ; i++) {
            if(i == 0)
                diag = n;
            else
                diag = ((n-i)*diag*(n+i))/(i*i);

            rest = diag*diag;
            hie[i][i] = rest/(2*i+1);
            for(j = i + 1 ; j < n ; j++) {
                rest = -((n-j)*rest*(n+j))/(j*j);
                hie[i][j] = hie[j][i] = rest/(i+j+1);
            }
        }

/* difference of exact inverse and the calculated one */
        printf("\n\nError in Hilbert matrix inverse N=%d",i);
        matrix_print(matrix_sub(Hi,Hie));

/* everything used in loop must be freed */
        matrix_free(H);
        matrix_free(Hi);
        matrix_free(Hie);
    }
}
