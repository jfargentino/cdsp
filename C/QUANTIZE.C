#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "disk.h"
#include "get.h"

/**************************************************************************

QUANTIZE.C - PROGRAM TO QUANTIZE A RECORD IN A DSP DATA FILE SO THAT
             THE MAXIMUM ABSOLUTE VALUE IS AN INTEGER ENTERED BY
             THE USER.  THE RESULTING DSP DATA FILE HAS THE
             QUANTIZED DATA AND THE ERROR OF THE QUANTIZATION AS
             TWO RECORDS.

INPUTS: INTEGER VALUE, FILE NAME.

OUTPUTS: DSP DATA FILE WITH TWO RECORDS

*************************************************************************/

main()
{
    DSP_FILE *dsp_in,*dsp_out;  /* input and output files */

    int i,int_sig,length,max_integer;
    float x,signal_max;       /* used to find absolute max */
    float *signal,*error;

    char *trail;
    static char trailer[300];

    dsp_in = open_read(get_string("input file name"));
    if(!dsp_in) exit(1);     /* bad filename */

    length = dsp_in->rec_len;

    trail = read_trailer(dsp_in);
    if(trail && *trail) printf("\nFile trailer:\n%s\n",trail);

    signal = read_float_record(dsp_in); /* read first record */

/* find max absolute signal value */
    signal_max = fabs(signal[0]);
    for(i = 1 ; i < length ; i++) {
        x = fabs(signal[i]);
        if(x > signal_max) signal_max = x;
    }

    printf("\nSignal absolute max = %f\n",signal_max);

    error = (float *)calloc(length,sizeof(float));
    if(!error) {
        printf("\nUnable to allocate error array\n");
        exit(1);
    }

    max_integer = get_int("max absolute integer value",1,32767);

/* quantize record and calculate error vector */
    for(i = 0 ; i < length ; i++) {
        x = (float)max_integer*signal[i]/signal_max;
        int_sig = ROUND(x);
        signal[i] = int_sig;
        error[i] = x - (float)int_sig;
    }

    dsp_out = open_write(get_string("output file name"),FLOAT,2,length);
    if(!dsp_out) exit(1);

/* write out the quantized data */
    write_record((char *)signal,dsp_out);

/* write out error data */
    write_record((char *)error,dsp_out);

/* make descriptive trailer and write to file */
    sprintf(trailer,
        "\nQuantized signal and error\nSignal max = %f, Int max = %d\n",
             signal_max,max_integer);

/* add on to old trailer and write */
    write_trailer(append_trailer(trailer,dsp_in),dsp_out);
}
