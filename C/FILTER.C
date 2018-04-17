#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/**************************************************************************

FILTER.C - Source code for filter functions

    fir_filter_array   FIR filter float array of data
    fir_filter_int     FIR filter int data array with int coefficients
    fir_filter         FIR filter floats sample by sample (real time)
    iir_filter         IIR filter floats sample by sample (real time)
    gaussian           Generate Gaussian noise
    uniform            Generate uniform noise

*************************************************************************/

/* FILTER INFORMATION STRUCTURE FOR FILTER ROUTINES */

typedef struct {
    unsigned int length;       /* size of filter */
    float *history;            /* pointer to history in filter */
    float *coef;               /* pointer to coefficients of filter */
} FILTER;

/**************************************************************************

fir_filter_array - Filter float array of data

Requires FILTER structure for coefficients.
Input and output arrays are of equal length
and are allocated by the calller.

void fir_filter_array(float *in,float *out,int in_len,FILTER *fir)

    in            float pointer to input array
    out           float pointer to output array
    in_len        length of input and output arrays
    FILTER *fir   pointer to FILTER structure

*************************************************************************/

void fir_filter_array(in,out,in_len,fir)
    float *in,*out;
    int in_len;
    FILTER *fir;
{
    int i,j,coef_len2,acc_length;
    float acc;
    float *in_ptr,*data_ptr,*coef_start,*coef_ptr,*in_end;

/* set up for coefficients */
    coef_start = fir->coef;
    coef_len2 = (fir->length + 1)/2;

/* set up input data pointers */
    in_end = in + in_len - 1;
    in_ptr = in + coef_len2 - 1;

/* initial value of accumulation length for startup */
    acc_length = coef_len2;

    for(i = 0 ; i < in_len ; i++) {

/* set up pointers for accumulation */

        data_ptr = in_ptr;
        coef_ptr = coef_start;

/* do accumulation and write result */

        acc = (*coef_ptr++) * (*data_ptr--);
        for(j = 1 ; j < acc_length ; j++)
            acc += (*coef_ptr++) * (*data_ptr--);
        *out++ = acc;

/* check for end case */

        if(in_ptr == in_end) {
            acc_length--;       /* one shorter each time */
            coef_start++;       /* next coefficient each time */
        }

/* if not at end, then check for startup, add to input pointer */

        else {
            if(acc_length < fir->length) acc_length++;
            in_ptr++;
        }
    }
}

/**************************************************************************

fir_filter_int - Filters int data array based on passed int coefficients.

The length of the input and output arrays are equal
and are allocated by the calller.
The length of the coefficient array is passed.
An integer scale factor (passed) is used to divide the accumulation result.

void fir_filter_int(int *in,int *out,int in_len,
                                  int *coef,int coef_len,int scale)

    in          integer pointer to input array
    out         integer pointer to output array
    in_len      length of input and output arrays
    coef        integer pointer to coefficient array
    coef_len    length of coeffient array
    scale       scale factor to divide after accumulation

No return value.

*************************************************************************/

void fir_filter_int(in,out,in_len,coef,coef_len,scale)
    int *in,*out,*coef;
    int coef_len,in_len,scale;
{
    int i,j,coef_len2,acc_length;
    long acc;
    int *in_ptr,*data_ptr,*coef_start,*coef_ptr,*in_end;

/* set up for coefficients */
    coef_start = coef;
    coef_len2 = (coef_len + 1)/2;

/* set up input data pointers */
    in_end = in + in_len - 1;
    in_ptr = in + coef_len2 - 1;

/* initial value of accumulation length for startup */
    acc_length = coef_len2;

    for(i = 0 ; i < in_len ; i++) {

/* set up pointer for accumulation */

        data_ptr = in_ptr;
        coef_ptr = coef_start;

/* do accumulation and write result with scale factor */

        acc = (long)(*coef_ptr++) * (*data_ptr--);
        for(j = 1 ; j < acc_length ; j++)
            acc += (long)(*coef_ptr++) * (*data_ptr--);
        *out++ = (int)(acc/scale);

/* check for end case */

        if(in_ptr == in_end) {
            acc_length--;       /* one shorter each time */
            coef_start++;       /* next coefficient each time */
        }

/* if not at end, then check for startup, add to input pointer */

        else {
            if(acc_length < coef_len) acc_length++;
            in_ptr++;
        }
    }
}

/**************************************************************************

fir_filter - Perform fir filtering sample by sample on floats

Requires FILTER structure for history and coefficients.
Returns one output sample for each input sample.  Allocates history
array if not previously allocated.

float fir_filter(float input,FILTER *fir)

    float input        new float input sample
    FILTER *fir        pointer to FILTER structure

Returns float value giving the current output.
Allocation errors cause an error message and a call to exit.

*************************************************************************/

float fir_filter(input,fir)
    float input;        /* new input sample */
    FILTER *fir;        /* pointer to FILTER structure */
{
    int i;
    float *hist_ptr,*hist1_ptr,*coef_ptr;
    float output;

/* allocate history array if not already allocated */
    if(!fir->history) {
        fir->history = (float *) calloc(fir->length-1,sizeof(float));
        if(!fir->history) {
            printf("\nUnable to allocate history array in fir_filter\n");
            exit(1);
        }
    }

    hist_ptr = fir->history;
    hist1_ptr = hist_ptr;                   /* use for history update */
    coef_ptr = fir->coef + fir->length - 1; /* point to last coef */

/* form output accumulation */
    output = *hist_ptr++ * (*coef_ptr--);
    for(i = 2 ; i < fir->length ; i++) {
        *hist1_ptr++ = *hist_ptr;            /* update history array */
        output += (*hist_ptr++) * (*coef_ptr--);
    }
    output += input * (*coef_ptr);           /* input tap */
    *hist1_ptr = input;                      /* last history */

    return(output);
}

/**************************************************************************

iir_filter - Perform IIR filtering sample by sample on floats

Implements cascaded direct form II second order sections.
Requires FILTER structure for history and coefficients.
The length in the filter structure specifies the number of sections.
The size of the history array is 2*iir->length.
The size of the coefficient array is 4*iir->length + 1 because
the first coefficient is the overall scale factor for the filter.
Returns one output sample for each input sample.  Allocates history
array if not previously allocated.

float iir_filter(float input,FILTER *iir)

    float input        new float input sample
    FILTER *iir        pointer to FILTER structure

Returns float value giving the current output.

Allocation errors cause an error message and a call to exit.

*************************************************************************/

float iir_filter(input,iir)
    float input;        /* new input sample */
    FILTER *iir;        /* pointer to FILTER structure */
{
    int i;
    float *hist1_ptr,*hist2_ptr,*coef_ptr;
    float output,new_hist,history1,history2;

/* allocate history array if different size than last call */

    if(!iir->history) {
        iir->history = (float *) calloc(2*iir->length,sizeof(float));
        if(!iir->history) {
            printf("\nUnable to allocate history array in iir_filter\n");
            exit(1);
        }
    }

    coef_ptr = iir->coef;                /* coefficient pointer */

    hist1_ptr = iir->history;            /* first history */
    hist2_ptr = hist1_ptr + 1;           /* next history */

    output = input * (*coef_ptr++);      /* overall input scale factor */

    for(i = 0 ; i < iir->length ; i++) {
        history1 = *hist1_ptr;           /* history values */
        history2 = *hist2_ptr;

        output = output - history1 * (*coef_ptr++);
        new_hist = output - history2 * (*coef_ptr++);    /* poles */

        output = new_hist + history1 * (*coef_ptr++);
        output = output + history2 * (*coef_ptr++);      /* zeros */

        *hist2_ptr++ = *hist1_ptr;
        *hist1_ptr++ = new_hist;
        hist1_ptr++;
        hist2_ptr++;
    }

    return(output);
}

/**************************************************************************

gaussian - generates zero mean unit variance Gaussian random numbers

Returns one zero mean unit variance Gaussian random numbers as a double.
Uses the Box-Muller transformation of two uniform random numbers to
Gaussian random numbers.

double gaussian()

*************************************************************************/

double gaussian()
{
    static int ready = 0;       /* flag to indicated stored value */
    static double gstore;        /* place to store other value */
    double v1,v2,r,fac,gaus;
    double uniform();

/* make two numbers if none stored */
    if(ready == 0) {
        do {
            v1 = 2.*uniform();
            v2 = 2.*uniform();
            r = v1*v1 + v2*v2;
        } while(r > 1.0);       /* make radius less than 1 */

/* remap v1 and v2 to two Gaussian numbers */
        fac = sqrt(-2.*log(r)/r);
        gstore = v1*fac;        /* store one */
        gaus = v2*fac;          /* return one */
        ready = 1;              /* set ready flag */
    }

    else {
        ready = 0;      /* reset ready flag for next pair */
        gaus = gstore;  /* return the stored one */
    }

    return(gaus);
}

/**************************************************************************

uniform - generates zero mean uniform random number from -0.5 to 0.5

Returns one zero mean uniformly distributed random number as a double.
No arguments, calls function rand() from the C library.

double uniform()

*************************************************************************/

/* this define gives the biggest value rand() will return and
is used only if not defined in a header file (non-ANSI
implementations of C may require this) */

#ifndef RAND_MAX
#define RAND_MAX 32767
#endif

double uniform()
{
    return((double)(rand() & RAND_MAX) / RAND_MAX - 0.5);
}
