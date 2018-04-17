#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "get.h"
#include "matrix.h"

/****************************************************************

MSEOFPIC:  FINDS THE MEAN SQUARED DIFFERENCE BETWEEN TWO IMAGES

INPUTS: TWO FILENAMES

OUTPUTS: MEAN SQUARED ERROR AND ROOT MEAN SQUARED ERROR.

*****************************************************************/

main()
{
  MATRIX    *REF, *TST;
  short int *refrow, *tstrow;
  int       num_rows, num_cols, row, column;
  double    error, sum_error_sq, mean_sq_error, rms;

  printf("\nThis program find the mean squared error of a test\n");
  printf("image with respect to a reference image.\n");

  do{
    REF = matrix_read(get_string("name of reference matrix"));
  }while(!REF);

  do{
    TST = matrix_read(get_string("name of test matrix"));
  }while(!TST);
  
  num_rows = REF->rows;
  num_cols = REF->cols;
  
  if (REF->element_size != sizeof(short int)){
    printf("\nError: Reference matrix is not integer type.\n");
    exit(1);
  }
  if (TST->element_size != sizeof(short int)){
    printf("\nError: Test matrix is not integer type.\n");
    exit(1);
  }
  if (TST->rows != num_rows || TST->cols != num_cols){
    printf("\nError: The matrix sizes do not match.\n");
    exit(1);
  }

  sum_error_sq = 0;

  for (row=0; row<num_rows; row++){
    refrow = (short int *)REF->ptr[row];
    tstrow = (short int *)TST->ptr[row];
    for (column=0; column<num_cols; column++){
      error = refrow[column] - tstrow[column];
      sum_error_sq += error*error;
    }
  }

  mean_sq_error = sum_error_sq/((double)num_rows*(double)num_cols);
  rms = sqrt(mean_sq_error);

  printf("\nThe mean squared error of the test matrix as compared");
  printf("\nto the reference matrix is    %14.8f.", mean_sq_error);
  printf("\nThe rms error is              %14.8f.", rms);
}
