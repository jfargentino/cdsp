#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "disk.h"
#include "get.h"

/**************************************************************************

MKWAVE.C - EXAMPLE OF USE OF FORMATTED DISK DATA ROUTINES
           AND GET USER INPUT FUNCTIONS.
           GENERATES A SAMPLED SIGNAL WHICH IS THE SUM OF COSINE WAVES.

INPUTS: LENGTH, NUMBER OF FREQUENCIES, FREQUENCY VALUES, AND FILE NAME.

OUTPUTS: DSP DATA FILE

*************************************************************************/

main()
{
    DSP_FILE *dsp_info;
    int i,status,nfreqs,length;
    float *signal;
    float freqs[20];        /* max 20 frequencies */
    char prompt[20];        /* used to prompt user for input */
    static char trailer[300];

    float *wave();

    length = get_int("number of samples to generate",2,10000);

    nfreqs = get_int("number of frequencies in sum",1,20);

    for(i = 0 ; i < nfreqs ; i++) {
        sprintf(prompt,"frequency #%d",i);
        freqs[i] = get_float(prompt,0.0,0.5);
    }

    signal = wave(length, freqs, nfreqs);

    dsp_info = open_write(get_string("file name"),FLOAT,1,length);
    if(!dsp_info) exit(1);

    write_record((char *)signal,dsp_info);

/* make descriptive trailer and write to file */

    sprintf(trailer,
        "\nSignal of length %d equal to the sum of %d\n",length,nfreqs);

    strcat(trailer,"cosine waves at the following frequencies:\n");

/* add on each frequency */
    for(i = 0 ; i < nfreqs ; i++)
        sprintf(trailer+strlen(trailer),"f/fs = %f\n",freqs[i]);

    puts(trailer);   /* send to display */

    write_trailer(trailer,dsp_info);    /* send to file */
}

/**************************************************************************

wave - make a signal equal to the sum of sine waves

float *wave(int length, float *freqs, int number)

    length        number of samples to generate
    freqs         pointer to float array of values to use to
                  generate each frequency (sampling rate = 1).
    number        number of frequencies to generate (length of
                  frequency array).

Returns a pointer to a array of floats (allocated by the routine)
which contain the samples of the signal.  A sampling rate of 1
is assumed by the routine.

Allocation errors cause a call to exit(1).

*************************************************************************/

float *wave( length, freq, number)
    int length;        /* number of samples to generate */
    float *freq;       /* pointer to frequency array */
    int number;        /* number of frequencies to generate */
{
    int i, j;
    double arg,twopi;
    float *samp;                    /* pointer to samples */

/* allocate the sample array and set to zero */
    samp = (float *) calloc(length,sizeof(float));
    if(!samp) {
        printf("\nUnable to allocate samples\n");
        exit(1);
    }

/* Loop over the number of frequencies create the samples */
    twopi = 8.0*atan(1.0);            /* calculate 2*PI */
    for (j = 0 ; j < number ; j++) {
        arg = twopi * freq[j];
/* Loop over the length of the array */
        for (i = 0 ; i < length ; i++)
            samp[i] += cos(i*arg);
    }

/* Normalize amplitude by the number of frequencies */
    for (i=0; i<length; i++)
        samp[i] = samp[i]/number;

    return(samp);
}
