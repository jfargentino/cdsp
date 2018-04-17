#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "get.h"

/********************************************************************

EDETECT: This program performs edge detection on an image in the DSP
         file format where each row in the image is held in a record
         in the file.

         The steps in the processing are:

         1. Convolve with a Gaussian blur kernel.
         2. Find edges with a vertical Sobel operator.
         3. Find edges with a horizontal Sobel operator.
         4. Choose highest magnitude of the Sobel operators.
         5. Detect edges using a threshold value.

INPUTS: FILENAME of DSP format image file

OUTPUTS: DSP format output file

*********************************************************************/

main()
{
  MATRIX    *IN, *OUT, *VS, *HS, *FIL;
  DSP_FILE  *outfile;

  short int **fil, *out, *vs, *hs;
  int       i, j;
  short int threshold,temp1, temp2, min, max;
  double    avg,size;

  do {
    IN = matrix_read(get_string("image file to be edge detected"));
  } while(!IN);

  if (IN->element_size != sizeof(short int)){
    printf("\nError:  Input file is not of integer type.\n");
    exit(1);
  }

  size = (double) IN->rows * (double) IN->cols;

/* Allocate convolution matrix of size 3x3 */
  FIL = matrix_allocate(3,3,sizeof(short int));
  fil = (short int **) FIL->ptr;

/* Set the values of the filter matrix to a Gaussian kernel */
  fil[0][0] = 1;  fil[0][1] = 2;  fil[0][2] = 1;
  fil[1][0] = 2;  fil[1][1] = 4;  fil[1][2] = 2;
  fil[2][0] = 1;  fil[2][1] = 2;  fil[2][2] = 1;

/* Perform the Gaussian convolution */
  OUT = convol2d(IN, FIL);

/* Free the memory associated with the original image matrix */
  matrix_free(IN);

/* Vertical Sobel Operator */
  fil[0][0] = 1;  fil[0][1] = 0;  fil[0][2] = -1;
  fil[1][0] = 2;  fil[1][1] = 0;  fil[1][2] = -2;
  fil[2][0] = 1;  fil[2][1] = 0;  fil[2][2] = -1;

/* Convolve the smoothed matrix with the vertical Sobel kernel */
  VS = convol2d(OUT, FIL);

/* Horizontal Sobel Operator */
  fil[0][0] = 1;   fil[0][1] = 2;   fil[0][2] = 1;
  fil[1][0] = 0;   fil[1][1] = 0;   fil[1][2] = 0;
  fil[2][0] = -1;  fil[2][1] = -2;  fil[2][2] = -1;

/* Convolve the smoothed matrix with the horizontal Sobel kernel */
  HS = convol2d(OUT, FIL);

/*  Take the larger of the magnitudes of the two matrices,
  over write the OUT matrix with the result,  while finding
  the min, max and average gradient values */

  min = 10000;
  max = 0;
  avg = 0.0;
  for (i=0; i < HS->rows; i++){
    hs  = (short int *)HS->ptr[i];
    vs = (short int *)VS->ptr[i];
    out = (short int *)OUT->ptr[i];
    for (j=0; j < HS->cols; j++){
      temp1 = abs(hs[j]);
      temp2 = abs(vs[j]);
      temp1 = temp1 > temp2 ? temp1 : temp2;
      min = (temp1<min) ? temp1 : min;
      max = (temp1>max) ? temp1 : max;
      avg += temp1;
      out[j] = temp1;
    }
  }

  avg = avg/size;

/* Compare the magnitudes of the edges to a threshold */

  printf("\nThe min, max and average of the gradients are:\n");
  printf("  Min = %8d\n", min);
  printf("  Max = %8d\n", max);
  printf("  Avg = %8.1f\n", avg);

/* Get the threshold value as a user input */
  threshold = get_int("value of detection threshold", min, max);

/* Compare each edge value in the edge matrix with the user supplied
   threshold and create detection matrix (white on black) */

  for (i=0; i < OUT->rows; i++){
    out = (short int *)OUT->ptr[i];
    for (j=0; j < OUT->cols; j++)
      out[j] = out[j] > threshold ? 255 : 0;
  }

/* Write the output matrix to disk */
  do {
    outfile = matrix_write(OUT,get_string("edge image file name"));
  } while(!outfile);
}
