#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "disk.h"
#include "get.h"
#include "filter.h"

/**************************************************************************

IIRFILT.C - PROGRAM TO IIR FILTER THE RECORDS IN A DSP DATA FILE
            AND GENERATE A DSP DATA FILE CONTAINING THE FILTERED DATA.

USER ENTERS TYPE OF FILTER AND FILE NAMES.

*************************************************************************/

main()
{
    DSP_FILE *dsp_in,*dsp_out;  /* input and output files */
    FILTER *iir_ptr;

    int i,j,type,length;
    float *signal;

    char *trail,*trail_add;

    dsp_in = open_read(get_string("input file name"));
    if(!dsp_in) exit(1);     /* bad filename */

    length = dsp_in->rec_len;

    trail = read_trailer(dsp_in);
    if(trail && *trail) printf("\nFile trailer:\n%s\n",trail);

/* get type of filter to use of several defined in filter.h */
    type = get_int("filter type (0 = LPF, 1 = HPF)",0,1);

    if(type) iir_ptr = &iir_hpf;
    else iir_ptr = &iir_lpf;

/* open output, 1 record of floats */
    dsp_out = open_write(get_string("output file name"),
                                  FLOAT,dsp_in->records,length);
    if(!dsp_out) exit(1);

/* read, filter and write out data */
    for(i = 0 ; i < dsp_in->records ; i++) {
        signal = read_float_record(dsp_in);

/* filter the data in place */
        for(j = 0 ; j < length ; j++)
            signal[j] = iir_filter(signal[j],iir_ptr);

        write_record((char *)signal,dsp_out);

        free((char *)signal);             /* free old data space */
        free((char *)iir_ptr->history);   /* free old history */
        iir_ptr->history = NULL;
    }

/* make descriptive trailer and write to file */
    if(type) trail_add = "\nFiltered with highpass 6th order IIR filter";
    else trail_add = "\nFiltered with lowpass 5th order IIR filter";

    write_trailer(append_trailer(trail_add,dsp_in),dsp_out);
}
