#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "matrix.h"
#include "get.h"

/*************************************************************

NONLIN2D: This program does min, max, or median filtering
          of an image using the function nonlin2d().

INPUTS: DSP format image file.

OUTPUTS: DSP format image file.

************************************************************/

main()
{
  MATRIX   *IN, *OUT;
  int      i, passes, inval, filtype, size;

  do{
    IN = matrix_read(get_string("name of input image file"));
  }while(!IN);

  printf("Enter the function desired:\n");
  printf("  1 -- Erosion of light areas (min)\n");
  printf("  2 -- Dilation of light areas (max)\n");
  printf("  3 -- Median filter of the image\n");

  inval = get_int("your choice", 1, 3);

  passes = get_int("number of passes", 1, 10);

  filtype = inval - 1;
  size = 3;

  for (i=0; i<passes; i++){
    OUT = nonlin2d(IN, size, filtype);
    i++;
    if (i < passes)
      IN = nonlin2d(OUT, size, filtype);
  }

  if (passes%2)
    matrix_write(OUT, get_string("filtered output file name"));
  else
    matrix_write(IN, get_string("filtered output file name"));
}
