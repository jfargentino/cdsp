#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>
#include  "get.h"
#include  "matrix.h"

/************************************************************

FLATTEN:  THIS PROGRAM USES THE FUNCTION histogram TO LEVEL
          THE HISTOGRAM OF AN INPUT IMAGE.
          
INPUTS:   IMAGE TO BE HISTOGRAM FLATTENED IN DSP FILE FORMAT

OUTPUTS:  NEW IMAGE WITH FLATTENED HISTOGRAM.

************************************************************/

main()
{
  DSP_FILE  *dsp_info;
  MATRIX    *IN, *OUT;
  float     *hist_array;
  double    image_size, temp_flt;
  int       i, j, new_gray_level[256], temp_int;
  short int *in, *out;
  char      *in_name, trail[100];

/* Read input file into a matrix structure.  */
  do{
    in_name = get_string("image file to be histogram flattened");
    IN = matrix_read(in_name);
  }while(!IN);

  if (IN->element_size != sizeof(short int)){
    printf("\nError:  Input file is not of integer type\n");
    exit(1);
  }

/* Calculate the number of pixels in the image.  */
  image_size = (double) IN->rows * (double) IN->cols;

/* Create an array of 256 floats which represent the number
   of pixels in the image at a particular gray level.    
   Write the array to disk as the first record in a 
   two record DSP file. */

  hist_array = histogram(IN,0,255);

  do{
    dsp_info = open_write(get_string("histogram file name"),FLOAT,2,256);
  }while(!dsp_info);

  write_record((char *)hist_array,dsp_info);

/* Using the histogram array create a mapping of the original
   gray levels to the new histogram flattened gray levels */

  temp_flt = 0.;
  for(i=0; i<256; i++){     /* Loop thru original gray levels */

/* Find the distribution function of the image at each gray level */
    temp_flt += hist_array[i]/image_size;

/* Use the distribution function to create the mapping 
   from old to new gray levels */
    new_gray_level[i] = (255.*temp_flt) + 0.5;    
  }  

/* Using the new mapping of gray levels create a new image
   from the original */
  OUT = matrix_allocate(IN->rows, IN->cols, sizeof(short int));

  for (i=0; i<IN->rows; i++){
    in  = (short int *)IN->ptr[i];
    out = (short int *)OUT->ptr[i];
    for (j=0; j<IN->cols; j++){
      temp_int = in[j];
      temp_int = temp_int>255 ? 255 : temp_int;
      temp_int = temp_int<0   ?   0 : temp_int;
      out[j] = new_gray_level[temp_int];
    }
  }

/* Find the histogram of the newly created image and write as the
   second record in the file which is already open */

  hist_array = histogram(OUT,0,255);

  write_record((char *)hist_array,dsp_info);

/* make descriptive trailer for output histogram file */
  sprintf(trail,
    "Histograms of file %s before and after flattening",in_name);
  write_trailer(trail,dsp_info);

/* Write the new image to disk */
  matrix_write(OUT, get_string("histogram flattened image name"));
}
