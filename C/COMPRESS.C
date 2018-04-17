#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "get.h"
#include "matrix.h"

/************************************************************

COMPRESS:   Program which  uses the Discrete Cosine Transform
            to reduce the amount of data needed to represent
            an image by a factor of 4:1 while maintaining
            maximum information content.  A block coding
            technique is used with a block size of 8x8.
            
INPUTS:     A image input file
            
OUTPUTS:    An output DSP file containing
            block transform encoded values.
            
***********************************************************/

main()
{
  int   in_row,in_col,out_length;

  unsigned char *out,*outptr;
  short int     **f;

  MATRIX  *IN, *F;
  DSP_FILE *dsp_info;

  unsigned char pack(short int,short int);
  
/* Get the input DSP file containing the image */
  do{
    IN = matrix_read(get_string("image file to be transform coded"));
  } while(!IN);

/* output record length */
  out_length = 2*IN->cols;

/* open output unsigned char DSP data file, rows/8 records */
  do {
    dsp_info = open_write(get_string("compressed output file name"),
            UNSIGNED_CHAR,IN->rows/8,out_length);
  } while(!dsp_info);
  
/* Allocate memory for the output records */
  out = (unsigned char *) calloc(out_length,sizeof(unsigned char));
  if(!out) {
      printf("\nOutput buffer allocation error\n");
      exit(1);
  }

/* Do 8x8 compression, retaining only 8 bit or 4 bits per coef */

  for (in_row=0 ; in_row < IN->rows ; in_row+=8) {

    printf("\nRow=%4d", in_row);

    outptr = out;
    for (in_col=0 ; in_col < IN->cols ; in_col+=8) {

      F = matrix_crop(IN, in_row, in_col, 8, 8);

      dct2d(F);

      f = (short int **)F->ptr;

      *outptr++ = f[0][0];
      *outptr++ = f[0][1];
      *outptr++ = pack(f[0][2]/3,f[0][3]/3);
      *outptr++ = pack(f[0][4]/2,f[0][5]/2);
      *outptr++ = pack(f[0][6],f[0][7]);
      *outptr++ = f[1][0];
      *outptr++ = f[1][1];
      *outptr++ = pack(f[1][2]/2,f[1][3]);
      *outptr++ = pack(f[1][4],f[1][5]);
      *outptr++ = pack(f[2][0]/3,f[2][1]/2);
      *outptr++ = pack(f[2][2]/2,f[2][3]);
      *outptr++ = pack(f[3][0]/3,f[3][1]);
      *outptr++ = pack(f[3][2],f[3][3]);
      *outptr++ = pack(f[4][0]/2,f[4][1]);
      *outptr++ = pack(f[5][0]/2,f[5][1]);
      *outptr++ = pack(f[6][0],f[7][0]);

      matrix_free(F);
    }
  write_record(out,dsp_info);
  }
}

/* pack function to pack two ints (-8 to 7) into an unsigned char */

unsigned char pack(a,b)
    short int a,b;
{
/* limit to signed 4 bits */
    if(a > 7) a = 7;
    if(a < -8) a = -8;
    if(b > 7) b = 7;
    if(b < -8) b = -8;
/* pack into 8 bit and return */
    return((unsigned char)((a << 4) | (b & 15)));
}
