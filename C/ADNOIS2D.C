#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "get.h"
#include "matrix.h"

/********************************************************************

ADNOIS2D.C - PROGRAM TO ADD NOISE TO EXISTING IMAGE DATA
             ADDS NOISE TO EACH RECORD OF INPUT FILE AND
             GENERATES AN OUTPUT FILE OF THE SAME SIZE.

INPUTS: TYPE AND AMPLITUDE OF NOISE, FILE NAMES.

OUTPUTS: DSP DATA FILE

*********************************************************************/

main()
{
    DSP_FILE  *dsp_out;
    MATRIX    *MAT;

    int       i,j,type;
    short int *signal;
    char      *type_str;
    char      trailer[150];
    double    temp_flt,nmult;
    double    gaussian(),uniform();

/* Get input file name and read in the input matrix */
    do{
      MAT  = matrix_read(get_string("input file name"));
    }while(!MAT);

    if (MAT->element_size != sizeof(short int)){
      printf("\nError: Reference matrix is not integer type\n");
      exit(1);
    }

/* get type of noise and noise amplitude */
    type = get_int("noise type (0= uniform, 1= gaussian, 2= spike)",0,2);
    nmult = get_float("noise multiplier",0.0,1000.0);

/* add noise to records */
    for (i = 0; i < MAT->rows; i++){
        signal = (short int *)MAT->ptr[i];
        switch (type){
          case 0 :
            for(j = 0 ; j < MAT->cols ; j++){ 
                temp_flt = signal[j] + nmult*uniform();
                signal[j] = ROUND(temp_flt);
            }
            type_str = "Uniform";
            break;

          case 1 :
            for(j = 0 ; j < MAT->cols ; j++){ 
                temp_flt = signal[j] + nmult*gaussian();
                signal[j] = ROUND(temp_flt);
            }
            type_str = "Gaussian";
            break;
   
          case 2 :
            for(j = 0 ; j < MAT->cols ; j++){ 
                temp_flt = gaussian();
                if(temp_flt > 2.0) signal[j] += nmult;
                if(temp_flt < -2.0) signal[j] -= nmult;
            }
            type_str = "Spike";
            break;
        }
    }

/* Write noisy matrix out to file */
    do{
      dsp_out = matrix_write(MAT, get_string("output file name"));
    }while(!dsp_out);

/* make descriptive trailer and write to file */
    sprintf(trailer,
        "\n2D noise signal, %s distribution, Amplitude = %f\n",
            type_str, nmult);

    puts(trailer);                     /* send to display */

    write_trailer(trailer,dsp_out);    /* send to file */
}
