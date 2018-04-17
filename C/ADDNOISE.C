#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "disk.h"
#include "get.h"

/**************************************************************************

ADDNOISE.C - PROGRAM TO ADD NOISE TO EXISTING DSP DATA OR CREATE
             NOISE ONLY.  ADDS NOISE TO FIRST RECORD OF INPUT FILE
             AND GENERATES A ONE RECORD OUTPUT FILE.

INPUTS: LENGTH, TYPE AND AMPLITUDE OF NOISE, FILE NAME.

OUTPUTS: DSP DATA FILE

*************************************************************************/

main()
{
    DSP_FILE *dsp_in,*dsp_out;  /* input and output files */

    int i,type,length;
    double nmult;           /* noise multiplier */
    float *signal;          /* pointer to record */

    char *file,*trail;
    static char trailer[300];

    double gaussian(),uniform();

    file = get_string("input file name (CR for none)");

/* check for file name: either generate or read a record */

    if(strlen(file) == 0) {
        length = get_int("number of samples to generate",2,32767);

        signal = (float *)calloc(length,sizeof(float));
        if(!signal) {
            printf("\nUnable to allocate noise array\n");
            exit(1);
        }
    }
    else {
        dsp_in = open_read(file);
        if(!dsp_in) exit(1);     /* bad filename */

        length = dsp_in->rec_len;

        trail = read_trailer(dsp_in);
        if(trail && *trail) printf("\nFile trailer:\n%s\n",trail);

        signal = read_float_record(dsp_in); /* read first record */
    }

/* get type of noise and noise amplitude */
    type = get_int("noise type (0 = uniform, 1 = Gaussian)",0,1);
    nmult = get_float("noise multiplier",0.0,1000.0);

/* add noise to record */
    if(type == 0)
        for(i = 0 ; i < length ; i++) signal[i] += nmult*uniform();
    else
        for(i = 0 ; i < length ; i++) signal[i] += nmult*gaussian();

    dsp_out = open_write(get_string("output file name"),FLOAT,1,length);
    if(!dsp_out) exit(1);

/* write out the noisy data */
    write_record((char *)signal,dsp_out);

/* make descriptive trailer and write to file */
    sprintf(trailer,
        "\nNoise signal of length %d, %s distribution, Amplitude = %f\n",
             length,type ? "Gaussian" : "Uniform",nmult);

    puts(trailer);   /* send to display */

    if(strlen(file) == 0) {
        write_trailer(trailer,dsp_out);    /* send to file */
    }

    else {          /* add on to old trailer and write */
        write_trailer(append_trailer(trailer,dsp_in),dsp_out);
    }
}
