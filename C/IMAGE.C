#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>
#include  "matrix.h"
#include  "get.h"

/**************************************************************************

IMAGE.C - Source code for image processing functions

dct2d       Discrete Cosine Transform in two dimensions
idct2d      Inverse Discrete Cosine Transform in two dimensions
histogram   Finds the histogram of a matrix
convol2d    Convolve a matrix with a filter kernel matrix
nonlin2d    Nonlinear filtering (min, max, median)

*************************************************************************/

/************************************************************

dct2d - Discrete Cosine Transform in two dimensions.

Performs the DCT and overwrites the input matrix
with the result.

void dct2d(MATRIX *IN)

************************************************************/

void dct2d(IN)
  MATRIX  *IN;
{
  int     m, u, v;

  MATRIX    *F, *G;
  float     **g, **cos1;
  short int **in;

  double  factor1, factor2, temp_flt;
  
  static MATRIX *COS1, *COS2;
  static int    len_store = 0;

  if (IN->rows != IN->cols){
    printf("\nError: Matrix to be transformed in dct2d not square\n");
    exit(1);
  }

  if (IN->element_size != sizeof(short int)){
    printf("\nError: Matrix is not of type integer in dct2d\n");
    exit(1);
  }

/* Create the cosine matrices if necessary */
  if (len_store != IN->rows){
  
/* If this is not the first creation of cosine matrices then must
   free the old cosine matrices */

    if (len_store != 0){
      matrix_free(COS1);
      matrix_free(COS2);
    }

    len_store = IN->rows;

    COS1 = matrix_allocate(IN->rows, IN->cols, sizeof(float));
    
    factor1 = 2.0*atan(1.0)/(double)IN->rows;

    cos1 = (float **)COS1->ptr;

/* scale cosine matrix by 1/N for DCT */
    for (u = 0; u < IN->rows; u++){
      factor2 = u*factor1;
      for (m = 0; m < IN->rows; m++) {
        cos1[u][m] = (float)cos((2*m + 1)*factor2);
        cos1[u][m] = cos1[u][m]/(float)IN->rows;
      }
    }

    COS2 = matrix_transpose(COS1);
  }    

/* Now calculate the DCT by matrix multiplies */

  F = matrix_mult(IN, COS2);
  G = matrix_mult(COS1, F);

/* Scale the result and put back in the integer input matrix */

  in = (short int **)IN->ptr;
  g  = (float **)G->ptr;

  for (u = 0; u < IN->rows; u++){
    for (v = 0; v < IN->cols; v++){
      temp_flt = g[u][v];
      in[u][v] = ROUND(temp_flt);
    }
  }

  matrix_free(F);
  matrix_free(G);
}

/************************************************************

idct2d - Inverse Discrete Cosine Transform in two dimensions.

Performs the inverse DCT and overwrites the input matrix
with the result.
        
void idct2d(MATRIX *IN)

************************************************************/

void idct2d(IN)
  MATRIX  *IN;
{

  int     m, u, v, coef1, coef2;

  MATRIX    *F, *G;
  float     **g, **cos1;
  short int **in;

  double  factor1, factor2, temp_flt;
  
  static MATRIX    *COS1, *COS2;
  static int       len_store = 0;

  if (IN->rows != IN->cols){
    printf("\nError: Matrix to be inverse transformed not square\n");
    exit(1);
  }

  if (IN->element_size != sizeof(short int)){
    printf("\nError: Matrix is not of type integer in idct2d\n");
    exit(1);
  }

/* Create the cosine matrices if necessary */

  if (len_store != IN->rows){
  
/* If this is not the first creation of cosine matrices then must
   free the old cosine matrices */

    if (len_store != 0){
      matrix_free(COS1);
      matrix_free(COS2);
    }

    len_store = IN->rows;

    COS1 = matrix_allocate(IN->rows, IN->cols, sizeof(float));
    
    factor1 = 2.0*atan(1.0)/(double)IN->rows;

    cos1 = (float **)COS1->ptr;

/* include DC scale factor in cosine matrix */
    for (u = 0; u < IN->rows; u++){
      factor2 = u*factor1;
      for (m = 0; m < IN->rows; m++) {
        if(u > 0) {
          cos1[u][m] = (float)2.0*cos((2*m + 1)*factor2);
        }
        else {
          cos1[u][m] = (float)cos((2*m + 1)*factor2);
        }
      }
    }

    COS2 = matrix_transpose(COS1);
  }    

/* Now calculate the IDCT by matrix multiplies */

  F = matrix_mult(IN, COS1);
  G = matrix_mult(COS2, F);

/* Put the result back in the integer input matrix */

  in = (short int **)IN->ptr;
  g  = (float **)G->ptr;

  for (u = 0; u < IN->rows; u++){
    for (v = 0; v < IN->cols; v++){
      temp_flt = g[u][v];
      in[u][v] = ROUND(temp_flt);
    }
  }

  matrix_free(F);
  matrix_free(G);
}

/*************************************************************

histogram - Finds the histogram of a matrix.

Returns an array whose elements are the histogram of the
image pointed to by the MATRIX pointer (must be integer
type).  Pixel values counted in the histogram are from the
parameter min_val to max_val.  Returns a pointer to an
array of floats allocated by the histogram function.

float *histogram(MATRIX *IN,int min_val,int max_val)

*************************************************************/

float *histogram(IN,min_val,max_val)
  MATRIX  *IN;
  int min_val,max_val;
{
  int       i, j, temp_val;
  float     *hist_array;
  short int *inrow;  
  
  if (IN->element_size != sizeof(short int)){
    printf("\nError: Input file is not of integer type\n");
    exit(1);
  }

/* Allocate memory for the histogram array and clear all
   locations which are allocated */
  
  hist_array = (float *)calloc(max_val-min_val+1, sizeof(float));
  if (!hist_array){
    printf("\nError: Unable to allocate histogram array\n");
    exit(1);   
  }

/* Step through the input matrix.  Make sure each value is within 
   the proper range for an index into the histogram array.  Then
   increment the histogram value at this index point */

  for (i=0; i<IN->rows; i++){
    inrow = (short int *)IN->ptr[i];
    for (j=0; j<IN->cols; j++){
      temp_val = inrow[j];
      temp_val = temp_val > max_val ? max_val : temp_val;
      temp_val = temp_val < min_val ? min_val : temp_val;
      hist_array[min_val+temp_val] += 1.0;
    }
  }

  return(hist_array);
}

/**************************************************************************

convol2d - Convolve a MATRIX with a filter kernel

Requires pointer to input MATRIX, pointer to filter kernel.
Returns pointer to output matrix.
Exits with error message if unable to allocate output matrix.

MATRIX *convol2d(MATRIX *IN,MATRIX *FIL)

*************************************************************************/

MATRIX *convol2d(IN, FIL)
  MATRIX  *IN, *FIL;
{
  MATRIX    *OUT;

  int       row, column, i, j, sumval, normal_factor;
  int       kernel_rows, kernel_cols, dead_rows, dead_cols;
  short int *outptr, *inptr, *filptr;

/* Both matrices must be integer type */

  if (IN->element_size != sizeof(short int)){
    printf("\nError: Input matrix in convol2d is not int type\n");
    exit(1);
  }

  if (FIL->element_size != sizeof(short int)){
    printf("\nError: Filter matrix in convol2d is not int type\n");
    exit(1);
  }

/* Allocate memory for the output matrix */

  OUT = matrix_allocate(IN->rows, IN->cols, IN->element_size);  

/* Set the size of the kernel rows and columns */ 
  kernel_rows = FIL->rows;
  kernel_cols = FIL->cols;

  dead_rows = kernel_rows/2;
  dead_cols = kernel_cols/2;
  
/* Calculate the normalization factor for the kernel matrix */
  normal_factor = 0;
  for (row=0; row<kernel_rows; row++){
    filptr = (short int *) FIL->ptr[row];
    for (column=0; column<kernel_cols; column++)
      normal_factor += abs(filptr[column]);
  }

/* Make sure the normalization factor is not 0 */
  if(!normal_factor) normal_factor = 1;

  printf("\n");
  for (row=0; row<(IN->rows)-kernel_rows+1; row++){

    if(row%16 == 0) printf("%4d",row);

    outptr = (short int *) OUT->ptr[row+dead_rows];
    for (column=0; column<(IN->cols)-kernel_cols+1; column++){

      sumval = 0;
      for (i=0; i<kernel_rows; i++){
        inptr = (short int *) IN->ptr[i+row];     /* row offset */
        inptr = inptr + column;             /* col offset */
        filptr = (short int *) FIL->ptr[i];
        for (j=0; j<kernel_cols; j++)
          sumval += (*inptr++) * (*filptr++);
      }
      
      outptr[column+dead_cols] = sumval/normal_factor;
      
    }
  }
  printf("\n");

  return(OUT);  
}

/********************************************************************

nonlin2d - Performs nonlinear filtering (min, max, median) on a
           matrix of data values using a square kernel.

Pointer to input MATRIX, kernel size, type of filter are
passed to nonlin2d function.

size is the number of rows or columns in the kernel
(must be odd). The type of filter function is 0 for min,
1 for max, 2 for median.

Pointer to output MATRIX is returned.
Returns NULL if unable to allocate output matrix
or if size not odd.

MATRIX *nonlin2d(MATRIX *IN,int size,int filtype)

*********************************************************************/

MATRIX *nonlin2d(IN, size, filtype)
  MATRIX  *IN;
  int     size, filtype;
{
  MATRIX *OUT;

  int       row, column, kernel_len;
  int       subrow, subcol, dead_rows, dead_cols;
  short int *outrow, *array, *arrptr, *subptr;
  int       int_cmp();

  if (IN->element_size != sizeof(short int)){
    printf("\nError:  Input file is not of integer type\n");
    exit(1);
  }

/* Allocate memory for the output matrix */
  OUT = matrix_allocate(IN->rows, IN->cols, IN->element_size);  

/* Check that the submatrix size has odd number of row and cols */
  if (!(size%2)) return(NULL);

  dead_rows = size/2;
  dead_cols = size/2;

/* Allocate memory for the sorting array */

  kernel_len = size*size;
  array = (short int *)calloc(kernel_len, sizeof(short int));
  if (!array) return(NULL);

/* Perform the sort on each submatrix in the input matrix */
  printf("\n");
  for (row=0; row<IN->rows-size+1; row++){

    outrow = (short int *) OUT->ptr[row+dead_rows];
    if (row%16==0) printf("%4d", row);

    for (column=0; column<IN->cols-size+1; column++){

      arrptr = array;
      for (subrow=0; subrow<size; subrow++){
        subptr = (short int *)IN->ptr[row + subrow];  /* row offset */
        subptr = subptr + column;                     /* col offset */
        for (subcol=0; subcol<size; subcol++)
          *arrptr++ = *subptr++;
      }

      qsort(array,kernel_len,sizeof(short int),int_cmp);

      switch(filtype){
        case 0 :
          outrow[column+dead_cols] = array[0];
          break;
        case 1 :
          outrow[column+dead_cols] = array[kernel_len-1];
          break;
        case 2 :
          outrow[column+dead_cols] = array[kernel_len/2];
          break;
        default :
          return(NULL);
      }
    }
  }

/* Free the sorting array.  */
  free(array);

  return(OUT);  
}

/* function to compare two ints for use with qsort in nonlin2d */

int int_cmp(a,b)
    short int *a,*b;
{
    return((*a) - (*b));
}
