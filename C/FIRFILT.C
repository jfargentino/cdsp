#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "disk.h"
#include "get.h"
#include "filter.h"

/**************************************************************************

FIRFILT.C - PROGRAM TO FIR FILTER THE RECORDS IN A DSP DATA FILE AND
            GENERATE A DSP DATA FILE CONTAINING THE NEW FILTERED DATA.

INPUTS: FILE NAMES, FILTER TYPE (LPF, HPF OR PULSE)

OUTPUTS: DSP DATA FILE WITH FILTERED DATA

*************************************************************************/

main()
{
    DSP_FILE *dsp_in,*dsp_out;  /* input and output files */
    FILTER *fir_ptr;

    int i,length,type;
    float *signal_in,*signal_out;

    char *trail,*trail_add;

/* open input file */
    dsp_in = open_read(get_string("input file name"));
    if(!dsp_in) exit(1);     /* bad filename */

    length = dsp_in->rec_len;

    trail = read_trailer(dsp_in);
    if(trail && *trail) printf("\nFile trailer:\n%s\n",trail);

    signal_out = (float *)calloc(length,sizeof(float));
    if(!signal_out) {
        printf("\nUnable to allocate output array\n");
        exit(1);
    }

/* get type of filter to use of several defined in filter.h */
    type = get_int("filter type (0 = LPF, 1 = HPF, 2 = PULSE)",0,2);

    if(type == 0) fir_ptr = &fir_lpf;    /* lowpass filter, 0.2 cutoff */
    if(type == 1) fir_ptr = &fir_hpf;    /* highpass filter, 0.3 cutoff */
    if(type == 2) fir_ptr = &fir_pulse;  /* matched pulse filter */

/* open output records of floats */
    dsp_out = open_write(get_string("output file name"),
                                  FLOAT,dsp_in->records,length);
    if(!dsp_out) exit(1);

/* read, filter and write out data */
    for(i = 0 ; i < dsp_in->records ; i++) {
        signal_in = read_float_record(dsp_in);
        fir_filter_array(signal_in,signal_out,length,fir_ptr);
        write_record((char *)signal_out,dsp_out);
        free((char *)signal_in);            /* free old space */
    }

/* make descriptive trailer and write to file */
    if(type == 0) trail_add = "\nLowpass filtered using FIRFILT";
    if(type == 1) trail_add = "\nHighpass filtered using FIRFILT";
    if(type == 2) trail_add = "\nMatched pulse filtered using FIRFILT";
    write_trailer(append_trailer(trail_add,dsp_in),dsp_out);
}
