#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "disk.h"
#include "get.h"
#include "filter.h"

/**************************************************************************

REALTIME.C - PROGRAM TO DEMONSTRATE REAL TIME FILTERING USING
             fir_filter() AND iir_filter().   A CHIRP SIGNAL IS GENERATED
             ONE SAMPLE AT A TIME BEFORE EACH FILTER CALL.

INPUTS: TYPE OF FILTER, OUTPUT FILE NAME.

OUTPUTS: DSP DATA FILE

*************************************************************************/

main()
{
    DSP_FILE *dsp_out;  /* output file */
    FILTER *filt_ptr;

    int i,type,length;
    float input;
    double arg,arg2;

    float *signal;
    char *trail;

    length = get_int("number of samples to generate",2,10000);

/* allocate the sample array and set to zero */
    signal = (float *) calloc(length,sizeof(float));
    if(!signal) {
        printf("\nUnable to allocate samples\n");
        exit(1);
    }

/* get type of filter to use of several defined in filter.h */
    printf("\nFilters available:\n");
    printf("\n0 - No Filter (chirp signal input)");
    printf("\n1 - FIR 35 point lowpass, Pass 0-0.2, Stop 0.25-0.5");
    printf("\n2 - FIR 35 point highpass, Pass 0.3-0.5, Stop 0-0.25");
    printf("\n3 - IIR Elliptic lowpass, Pass 0-0.2, Stop 0.25-0.5");
    printf("\n4 - IIR Chebyshev highpass, Pass 0.3-0.5, Stop 0-0.25");

    type = get_int("filter type",0,4);

    if(type == 1) filt_ptr = &fir_lpf;
    if(type == 2) filt_ptr = &fir_hpf;
    if(type == 3) filt_ptr = &iir_lpf;
    if(type == 4) filt_ptr = &iir_hpf;

    arg = 2.0*atan(1.0)/(double)length; /* calculate PI/2*length */
/* generate and filter the data */
    for(i = 0 ; i < length ; i++) {
        input = sin((double)i * (double)i * arg);
        if(type == 0) signal[i] = input;
        if(type == 1 || type == 2) signal[i] = fir_filter(input,filt_ptr);
        if(type >= 3)  signal[i] = iir_filter(input,filt_ptr);
    }   

/* open output, 1 record of floats */
    dsp_out = open_write(get_string("output file name"),FLOAT,1,length);
    if(!dsp_out) exit(1);

/* write out the filtered data */
    write_record((char *)signal,dsp_out);

/* make descriptive trailer and write to file */
    if(type == 0) trail = "Chirp Signal without filter";
    if(type == 1) trail = "Chirp after FIR 35 point lowpass filter";
    if(type == 2) trail = "Chirp after FIR 35 point highpass filter";
    if(type == 3) trail = "Chirp after IIR Elliptic lowpass filter";
    if(type == 4) trail = "Chirp after IIR Chebyshev highpass";

    write_trailer(trail,dsp_out);
}
