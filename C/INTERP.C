#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "disk.h"
#include "get.h"
#include "filter.h"

/**************************************************************************

INTERP.C - PROGRAM TO DEMONSTRATE 2:1 AND 3:1 FIR FILTER INTERPOLATION
           USES TWO INTERPOLATION FILTERS AND MULTIPLE CALLS TO THE
           REAL TIME FILTER FUNCTION fir_filter().

INPUTS: INPUT FILE NAME, INTERPOLATION RATIO (2 OR 3), OUTPUT FILE NAME.

OUTPUTS: DSP DATA FILE

*************************************************************************/

main()
{
    DSP_FILE *dsp_in,*dsp_out;  /* input and output files */
    FILTER interp_filt1,interp_filt2;

    int i,ratio,length,out_len;

    float *signal_in,*signal_out;
    char *trail_add,*trail;

/* interpolation coefficients for the decimated filters */
    float coef2[16];
    float coef31[16],coef32[16];

/* 2:1 interpolation coefficients, PB 0-0.2, SB 0.3-0.5 */
    static float interp2[31] = {
  -0.00258821, 0.,  0.00748852, 0., -0.01651064, 0.,  0.03176119, 0.,
  -0.05692563, 0.,  0.10079526, 0., -0.19532167, 0.,  0.63082207, 1.0,
   0.63082207, 0., -0.19532167, 0.,  0.10079526, 0., -0.05692563, 0.,
   0.03176119, 0., -0.01651064, 0.,  0.00748852, 0., -0.00258821
              };

/* 3:1 interpolation coefficients, PB 0-0.133, SB 0.2-0.5 */
    static float interp3[47] = {
  -0.00178662, -0.00275941, 0.,  0.00556927,  0.00749929, 0.,
  -0.01268113, -0.01606336, 0.,  0.02482278,  0.03041984, 0.,
  -0.04484686, -0.05417098, 0.,  0.07917613,  0.09644332, 0.,
  -0.14927754, -0.19365910, 0.,  0.40682136,  0.82363913, 1.0,
   0.82363913,  0.40682136, 0., -0.19365910, -0.14927754, 0.,
   0.09644332,  0.07917613, 0., -0.05417098, -0.04484686, 0.,
   0.03041984,  0.02482278, 0., -0.01606336, -0.01268113, 0.,
   0.00749928,  0.00556927, 0., -0.00275941, -0.00178662
              };


/* open input file and read the first record */
    dsp_in = open_read(get_string("input file name"));
    if(!dsp_in) exit(1);     /* bad filename */

    length = dsp_in->rec_len;

    trail = read_trailer(dsp_in);
    if(trail && *trail) printf("\nFile trailer:\n%s\n",trail);

    signal_in = read_float_record(dsp_in); /* read first record */

/* get interpolation ratio (2 or 3) */
    ratio = get_int("interpolation ratio",2,3);

/* decimate the coefficients of one of the filters and make structures */
    if(ratio == 2) {
        interp_filt1.length = 16;
        interp_filt1.history = NULL;
        interp_filt1.coef = coef2;  /* pointer to 16 values */
        for(i = 0 ; i < 16 ; i++) coef2[i] = interp2[2*i];
    }
    else {          /* ratio = 3 case */
        interp_filt1.length = 16;
        interp_filt1.history = NULL;
        interp_filt1.coef = coef31;  /* pointer to 16 values */
        for(i = 0 ; i < 16 ; i++) coef31[i] = interp3[3*i];

        interp_filt2.length = 16;
        interp_filt2.history = NULL;
        interp_filt2.coef = coef32;  /* pointer to 16 values */
        for(i = 0 ; i < 16 ; i++) coef32[i] = interp3[3*i+1];
    }

    out_len = ratio*length;

/* allocate the sample array and set to zero */
    signal_out = (float *) calloc(out_len,sizeof(float));
    if(!signal_out) {
        printf("\nUnable to allocate output samples\n");
        exit(1);
    }

/* interpolate the data by calls to fir_filter */
    if(ratio == 2) {    /* make two samples for each input */
        for(i = 0 ; i < length ; i++) {
            if((i-8) > 0) signal_out[2*i] = signal_in[i-8];
            signal_out[2*i+1] = fir_filter(signal_in[i],&interp_filt1);
        }
    }
    else {              /* make three samples for each input */
        for(i = 0 ; i < length ; i++) {
            if((i-8) > 0) signal_out[3*i] = signal_in[i-8];
            signal_out[3*i+1] = fir_filter(signal_in[i],&interp_filt1);
            signal_out[3*i+2] = fir_filter(signal_in[i],&interp_filt2);
        }
    }

/* open output, 1 record of floats */
    dsp_out = open_write(get_string("output file name"),FLOAT,1,out_len);
    if(!dsp_out) exit(1);

/* write out the filtered data */
    write_record((char *)signal_out,dsp_out);

/* make descriptive trailer and write to file */
    if(ratio == 2) trail_add = "\nFIR interpolated 2:1";
    else trail_add = "\nFIR interpolated 3:1";

    write_trailer(append_trailer(trail_add,dsp_in),dsp_out);
}
