#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "disk.h"
#include "get.h"
#include "filter.h"

/**************************************************************************
  
REALCMX.C - PROGRAM TO CONVERT AN RF DATA RECORD TO A DSP DATA
            FILE CONTAINING TWO RECORDS REPRESENTING THE REAL
            AND IMAGINARY PART OF THE SIGNAL.
   
INPUTS: FILE NAMES.
  
OUTPUTS: DSP DATA FILE WITH COMPLEX DATA

*************************************************************************/

main()
{
    DSP_FILE *dsp_in,*dsp_out;  /* input and output files */

/* 35 point hilbert transform FIR filter cutoff at 0.02 and 0.48
   +/- 0.5 dB ripple in passband, zeros at 0 and 0.5 */

    static float  fir_hilbert35[35] = {
    0.038135,    0.000000,    0.024179,    0.000000,    0.032403,
    0.000000,    0.043301,    0.000000,    0.058420,    0.000000,
    0.081119,    0.000000,    0.120167,    0.000000,    0.207859,
    0.000000,    0.635163,    0.000000,   -0.635163,    0.000000,
   -0.207859,    0.000000,   -0.120167,    0.000000,   -0.081119,
    0.000000,   -0.058420,    0.000000,   -0.043301,    0.000000,
   -0.032403,    0.000000,   -0.024179,    0.000000,   -0.038135
                          };

    static FILTER fir_hilbert = {
        35,
        NULL,
        fir_hilbert35
    };

    int i,length,decim;
    float *signal_in,*signal_out;

    char *trail;
    char trail_add[200];

    dsp_in = open_read(get_string("input file name"));
    if(!dsp_in) exit(1);     /* bad filename */

    length = dsp_in->rec_len;

    trail = read_trailer(dsp_in);
    if(trail && *trail) printf("\nFile trailer:\n%s\n",trail);

    signal_in = read_float_record(dsp_in); /* read first record */

    signal_out = (float *)calloc(length,sizeof(float));
    if(!signal_out) {
        printf("\nUnable to allocate output array\n");
        exit(1);
    }

    decim = get_int("decimation ratio",1,20);

    fir_filter_array(signal_in,signal_out,length,&fir_hilbert);

/* decimate original input and Hilbert transformed output */
    for(i = 1 ; i < (length+decim-1)/decim ; i++) {
        signal_in[i] = signal_in[i*decim];
        signal_out[i] = signal_out[i*decim];
    }   

/* open output, 2 records of floats */
    dsp_out = open_write(get_string("output file name"),
                                                FLOAT,2,length/decim);
    if(!dsp_out) exit(1);

/* write out the original data after decimation */
    write_record((char *)signal_in,dsp_out);

/* write out the filtered data after decimation */
    write_record((char *)signal_out,dsp_out);

/* make descriptive trailer and write to file */
    sprintf(trail_add,
        "\nConverted to complex using REALCMX.C, Decimated %d:1",decim);
    write_trailer(append_trailer(trail_add,dsp_in),dsp_out);
}
