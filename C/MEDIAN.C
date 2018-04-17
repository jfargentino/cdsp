#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "disk.h"
#include "get.h"

/**************************************************************************

MEDIAN.C - PROGRAM TO CONDITIONALLY MEDIAN FILTER A RECORD AND GENERATE
           FILE CONTAINING THE NEW FILTERED DATA.

INPUTS: MEDIAN FILTER THRESHOLD AND LENGTH, FILE NAMES.

OUTPUTS: DSP DATA FILE

*************************************************************************/

main()
{
    DSP_FILE *dsp_in,*dsp_out;  /* input and output files */

    int i,length,median_length;
    double thres;
    float *signal_in,*signal_out;

    char *trail,*trail_add;

    void median_filter_array(float *,float *,int,int,double);

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

    median_length = get_int("median filter length",3,length);

    thres = get_float("conditional median threshold",0.0,1.e10);

    median_filter_array(signal_in,signal_out,length,median_length,thres);

/* open output, 1 record of floats */
    dsp_out = open_write(get_string("output file name"),FLOAT,1,length);
    if(!dsp_out) exit(1);

/* write out the filtered data */
    write_record((char *)signal_out,dsp_out);

/* make descriptive trailer and write to file */
    trail_add = "Filtered using MEDIAN filter";
    write_trailer(append_trailer(trail_add,dsp_in),dsp_out);
}


/**************************************************************************

median_filter_array - function to conditionally median filter a
                      float array.

Requires input arrays and output arrays, length
of input array, and length of median filter.  If the absolute
difference of the median and the center sample exceed the threshold
parameter, then the sample is replaced by the median (a
threshold of zero will give straight median filtering).

void median_filter_array(float *in,float *out,int in_len,
                               int median_len,double threshold)

    in            float pointer to input array
    out           float pointer to output array
    in_len        length of input and output arrays
    median_len    length of median filter to use (normally odd)
    threshold     median filter conditional threshold

*************************************************************************/

void median_filter_array(in,out,in_len,median_len,threshold)
    float *in,*out;
    int in_len,median_len;
    double threshold;
{
    int i,j,mlen2;
    float *sort_array;       /* pointer to array for data to sort */
    int float_cmp();

/* allocate the array to sort with */
    sort_array = (float *) calloc(median_len,sizeof(float));
    if(!sort_array) {
        printf("\nError allocating sort array in median_filter_array\n");
        exit(1);
    }

/* copy input to output to start with */
    for(i = 0 ; i < in_len ; i++) out[i] = in[i];

    mlen2 = median_len/2;

    for(i = 0 ; i < (in_len - median_len) ; i++) {

/* copy data to sort */
        for(j = 0 ; j < median_len ; j++) sort_array[j] = in[i+j];

/* sort the data using qsort standard library routine */
        qsort(sort_array,median_len,sizeof(float),float_cmp);

/* replace with median if difference greater than threshold */
        if(fabs(sort_array[mlen2] - in[i+mlen2]) > threshold)
            out[i+mlen2] = sort_array[mlen2];
    }

    free((char *)sort_array);      /* all done with sort array */
}

/* function to compare two floats for use with qsort */

int float_cmp(a,b)
    float *a,*b;
{
    if(*a > *b) return(1);
    else return(-1);
}
