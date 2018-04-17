#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "get.h"

/**************************************************************************

LSFIT.C - WEIGHTED LEAST SQUARES FIT EXAMPLE USE OF MATRIX FUNCTIONS

INPUTS: FIT ORDER, FILENAME OF (X,Y) INPUT AND OPTIONAL WEIGHT DATA.

OUTPUTS: PRINT OF SOLUTION OF LEAST SQUARES FIT AND DSP DATA FILE
         WITH Y BEST FIT, AND Y ERROR.

*************************************************************************/

main()
{
    MATRIX *At,*B,*At_Ai_At,*Z,*OUT;
    DSP_FILE *dsp_out,*xyw;
    int i,j,l,n;
    float *x,*y,*w;
    double **at,**b,**z,**out;
    double yfit,xpow;
    static char trailer[100];

/* open x , y , and w (optional) input file */
    xyw = open_read(get_string("x,y,w input file name"));
    if(!xyw) exit(1);     /* bad filename */

/* size of x, y and w vectors */
    n = xyw->rec_len;

/* read in x and y */
    x = read_float_record(xyw);
    y = read_float_record(xyw);

/* if weights are present use them, otherwise constant weights */
    if(xyw->records > 2) {
        w = read_float_record(xyw);
    }
    else {
        w = (float *)calloc(n,sizeof(float));
        if(!w) {
            printf("\nUnable to allocate w array\n");
            exit(1);
        }
        for(i = 0 ; i < n ; i++) w[i] = 1.0;
    }

    l = get_int("order of fit",1,20);

/* weighted powers of x matrix transpose = At */
    At = matrix_allocate(l+1,n,sizeof(double));
    at = (double **) At->ptr;

    for(i = 0 ; i < n ; i++) {
        at[0][i] = w[i];
        for(j = 1 ; j < (l + 1) ; j++)
            at[j][i] = at[j-1][i] * x[i];
    }

/* Z = weighted y vector */
    Z = matrix_allocate(n,1,sizeof(double));
    z = (double **) Z->ptr;
    for(i = 0 ; i < n ; i++) z[i][0] = w[i] * y[i];

    At_Ai_At = matrix_mult(matrix_invert(matrix_mult(At,
                           matrix_transpose(At))),At);

    B = matrix_mult(At_Ai_At,Z);

    printf("\nLeast squares solution column vector:");
    matrix_print(B);
    b = (double **)B->ptr;

/* make a matrix with the fit y and the difference */
    OUT = matrix_allocate(2,n,sizeof(double));
    out = (double **) OUT->ptr;

/* calculate the least squares y values */
    for(i = 0 ; i < n ; i++) {
        yfit = b[0][0];
        xpow = x[i];
        for(j = 1 ; j <= l ; j++) {
            yfit += b[j][0]*xpow;
            xpow = xpow * x[i];
        }
        out[0][i] = yfit;
        out[1][i] = y[i]-yfit;
    }

/* write the matrix out */
    do {
        dsp_out = matrix_write(OUT,get_string("y, fit, error file name"));
    } while(!dsp_out);

    sprintf(trailer,
        "\nLSFIT.C program, Order = %d\ny fit and y error\n",l);

/* write trailer to the data file */
    write_trailer(append_trailer(trailer,xyw),dsp_out);
}
