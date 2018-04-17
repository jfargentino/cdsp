#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "get.h"

/****************************************************************

SMOOTH:  CONVOLVES A MATRIX WITH A 3X3 GAUSSIAN FILTER KERNEL

INPUTS: FILENAMES

OUTPUTS: DSP format output file

*****************************************************************/

main()
{
  MATRIX    *IN, *OUT, *FIL;
  DSP_FILE  *outfile;
  short int **fil;

  do {
    IN = matrix_read(get_string("image file name"));
  } while(!IN);

  printf("\nImage is %3d rows by %3d columns\n",IN->rows,IN->cols);

  FIL = matrix_allocate(3,3,sizeof(short int));
  fil = (short int **) FIL->ptr;

  fil[0][0] = 1;  fil[0][1] = 2;  fil[0][2] = 1;
  fil[1][0] = 2;  fil[1][1] = 4;  fil[1][2] = 2;
  fil[2][0] = 1;  fil[2][1] = 2;  fil[2][2] = 1;

  OUT = convol2d(IN, FIL);

  do {
    outfile = matrix_write(OUT, get_string("name of output image"));
  } while(!outfile);
}
