#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "get.h"
#include "matrix.h"

/************************************************************

EXPAND:  Program to demonstrate the operation of expansion
         of an image coded using dct2d and COMPRESS.C.
         The encoded image is decoded using a 8x8 block
         transform technique and the idct2d function.

INPUTS:  Compressed image in DSP file format.

OUTPUTS: Recovered (decoded) image in DSP file format.

************************************************************/

main()
{
  int   i,j,out_row,out_col;

  short int   **out,**f;
  unsigned char *in,*inptr;

  MATRIX  *OUT, *F;
  DSP_FILE *dsp_in;

  short int unpack(unsigned char);

/* open input file */
  dsp_in = open_read(get_string("input file name"));
  if(!dsp_in) exit(1);     /* bad filename */

  if(dsp_in->type != UNSIGNED_CHAR) {
    printf("\nIncorect compressed file data type\n");
    exit(1);
  }

  printf("\nCompressed image size: %dx%d\n",
                            dsp_in->records,dsp_in->rec_len);

/* Allocate memory for the output records */
  in = (unsigned char *)calloc(dsp_in->rec_len,sizeof(char));
  if(!in) {
      printf("\nInput buffer allocation error\n");
      exit(1);
  }

/* Allocate memory for the decoded output matrix, set to zero */
  OUT = matrix_allocate(8*dsp_in->records,
            dsp_in->rec_len/2,sizeof(short int));

/* use F as pointers to submatrix in output matrix */
  F = matrix_allocate(8, 8, sizeof(short int));
  f = (short int **)F->ptr;

/* will not use the data space for the matrix, so free it */
  for(i = 0 ; i < 8 ; i++) free((char *)f[i]);

/* Perform the decoding algorithm */
  out = (short int **)OUT->ptr;
  for (out_row = 0 ; out_row < OUT->rows; out_row+=8) {

    printf("  Row=%4d.\n", out_row);

/* read compressed record representing 8 rows of image */
    read_record(in,dsp_in);
    inptr = in;

    for (out_col=0; out_col < OUT->cols; out_col+=8) {

/* reset output pointers */
      for(i = 0 ; i < 8; i++)
          f[i] = out[out_row + i] + out_col;

/* unpack 16 bytes for each 8x8 block */
      f[0][0] = *inptr++;
      f[0][1] = (signed char)(*inptr++);
      f[0][2] = 3*unpack(*inptr >> 4);
      f[0][3] = 3*unpack(*inptr++);
      f[0][4] = 2*unpack(*inptr >> 4);
      f[0][5] = 2*unpack(*inptr++);
      f[0][6] = unpack(*inptr >> 4);
      f[0][7] = unpack(*inptr++);

      f[1][0] = (signed char)(*inptr++);
      f[1][1] = (signed char)(*inptr++);

      f[1][2] = 2*unpack(*inptr >> 4);
      f[1][3] = unpack(*inptr++);
      f[1][4] = unpack(*inptr >> 4);
      f[1][5] = unpack(*inptr++);

      f[2][0] = 3*unpack(*inptr >> 4);
      f[2][1] = 2*unpack(*inptr++);
      f[2][2] = 2*unpack(*inptr >> 4);
      f[2][3] = unpack(*inptr++);
      f[3][0] = 3*unpack(*inptr >> 4);
      f[3][1] = unpack(*inptr++);
      f[3][2] = unpack(*inptr >> 4);
      f[3][3] = unpack(*inptr++);

      f[4][0] = 2*unpack(*inptr >> 4);
      f[4][1] = unpack(*inptr++);
      f[5][0] = 2*unpack(*inptr >> 4);
      f[5][1] = unpack(*inptr++);
      f[6][0] = unpack(*inptr >> 4);
      f[7][0] = unpack(*inptr++);

      idct2d(F);

    }
  }

/* Write the matrix out to disk in the DSP file format */
  matrix_write(OUT, get_string("name of expanded image file"));
}

/* unpack function converts lower 4 bits to a signed int -8 to 7 */
short int unpack(in)
    unsigned char in;
{
    in = in & 15;       /* only lower 4 bits */
    if(in > 7)
        return((short int)in - 16);    /* negative case */
    else
        return((short int)in);         /* positive case */
}
