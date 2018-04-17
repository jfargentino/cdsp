#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "disk.h"
#include "get.h"
#include "filter.h"

/**************************************************************************

IIRDEZN.C - PROGRAM TO MAKE IIR FILTER COEFFICIENTS FROM ANALOG
            PROTOTYPE COEFFICIENTS AND DETERMINE MAGNITUDE RESPONSE.

INPUTS: FILTER COEFFICIENTS, SAMPLING RATE, CUTOFF FREQUENCY.

OUTPUTS: FILTER COEFFICIENTS, DSP DATA FILE WITH MAGNITUDE

*************************************************************************/

#define MAG_LENGTH 501       /* size of magnitude vector to write */

main()
{
    DSP_FILE *dsp_out;
    FILTER iir;

    int i;
    double fs,fc,k;
    double a0,a1,a2,b0,b1,b2;
    int sections;
    char *cptr;

    double *mag;    /* filter magnitude */
    float *coef;    /* final coefficients */

    static char trailer[700];

    void prewarp(double *,double *,double *,double,double);
    void bilinear(double,double,double,double,double,double,
                    double *,double,float *);
    double *magnitude(FILTER *);

    fs = get_float("sampling rate",0.0,1.e9);

    fc = get_float("filter cutoff frequency",0.0,fs/2.);

    sections = get_int("number of sections",1,20);

    iir.length = sections;

/* allocate the coefficient array */
    iir.coef = (float *) calloc(4*sections+1,sizeof(float));
    if(!iir.coef) {
        printf("\nUnable to allocate coef array\n");
        exit(1);
    }

    coef = iir.coef + 1;

    k = 1.0;                /* start gain at 1 */

    for(i = 1 ; i <= sections ; i++) {
        printf("\nEnter section #%d normalized coefficients --",i);
        a0 = get_float("numerator A0",-1.e6,1.e6);
        a1 = get_float("numerator A1",-1.e6,1.e6);
        a2 = get_float("numerator A2",-1.e6,1.e6);

        prewarp(&a0,&a1,&a2,fc,fs);

        b0 = get_float("denominator B0",-1.e6,1.e6);
        b1 = get_float("denominator B1",-1.e6,1.e6);
        b2 = get_float("denominator B2",-1.e6,1.e6);

        prewarp(&b0,&b1,&b2,fc,fs);

        bilinear(a0,a1,a2,b0,b1,b2,&k,fs,coef);
        coef += 4;
    }

/* store gain constant */
    iir.coef[0] = k;

/* display coefficients */
    printf("\n\nZ domain IIR coefficients are as follows:");
    for(i = 0 ; i < (4*sections + 1) ; i++)
         printf("\nC[%d] = %15.10f",i,iir.coef[i]);

/* open output magnitude file */
    dsp_out = open_write(get_string("file name"),DOUBLE,2,MAG_LENGTH);
    if(!dsp_out) exit(1);

/* determine magnitude transfer function */
    mag = magnitude(&iir);

/* write out the magnitude data */
    write_record((char *)mag,dsp_out);

/* determine log of magnitude in dB and write out as second record */
    for(i = 0 ; i < MAG_LENGTH ; i++)
        mag[i] = 20.0*log10(MAX(mag[i],1.e-4));
    write_record((char *)mag,dsp_out);

/* make descriptive trailer and write to file */
    sprintf(trailer,
        "\nFrequency response of IIR filter, mag and 20*log(mag)\n");
    cptr = strchr(trailer,'\0');
    sprintf(cptr,"\nFc = %g Fs = %g\n",fc,fs);
    cptr = strchr(trailer,'\0');
    sprintf(cptr,"\nZ domain IIR coefficients are as follows:");
    for(i = 0 ; i < (4*sections + 1) ; i++) {
        cptr = strchr(trailer,'\0');
        sprintf(cptr,"\nC[%d] = %15.10f",i,iir.coef[i]);
    }
    write_trailer(trailer,dsp_out);
}

/* function to pre-warp the coefficients of a numerator or
denominator */

void prewarp(a0,a1,a2,fc,fs)
    double *a0,*a1,*a2;
    double fc,fs;
{
    double wp,pi;

    pi = 4.0*atan(1.0);
    wp = 2.0*fs*tan(pi*fc/fs);

    *a2 = (*a2)/(wp*wp);
    *a1 = (*a1)/wp;
}

/* function to bilinear transform the numerator and denominator
coefficients and return four z transform coefficients for direct
form II realization using the iir filter program */

void bilinear(a0,a1,a2,b0,b1,b2,k,fs,coef)
    double a0,a1,a2;     /* numerator coefficients */
    double b0,b1,b2;     /* denominator coefficients */
    double *k;           /* overall gain factor */
    double fs;           /* sampling rate */
    float *coef;         /* pointer to 4 iir coefficients */
{
    double ad,bd;

/* alpha denominator */
    ad = 4.*a2*fs*fs + 2.*a1*fs + a0;
/* beta denominator */
    bd = 4.*b2*fs*fs + 2.*b1*fs + b0;

/* update gain constant for this section */
    *k *= ad/bd;

/* bilinear transform the coefficients */
    *coef++ = (2.*b0 - 8.*b2*fs*fs)/bd;         /* beta1 */
    *coef++ = (4.*b2*fs*fs - 2.*b1*fs + b0)/bd; /* beta2 */
    *coef++ = (2.*a0 - 8.*a2*fs*fs)/ad;         /* alpha1 */
    *coef = (4.*a2*fs*fs - 2.*a1*fs + a0)/ad;   /* alpha2 */
}

/* function to find the magnitude of a iir filter 
returns a double vector with the magnitude from 0 to .5 */

double *magnitude(iir)
    FILTER *iir;
{
    int i,j;
    double arg,twopi,alpha1,alpha2,beta1,beta2,z1r,z2r,z1i,z2i;
    double f,n_real,n_imag,d_real,d_imag,real,imag;
    double *mag;
    float *coef;

    mag = (double *)calloc(MAG_LENGTH,sizeof(double));
    if(!mag) {
        printf("\nUnable to allocate magnitude vector\n");
        exit(1);
    }

/* loop through all the frequencies */

    twopi = 8.0*atan(1.0);            /* calculate 2*PI */
    for(i = 0 ; i < MAG_LENGTH ; i++) {

        f = (double)i/(2*(MAG_LENGTH-1));
        arg = twopi*f;
        z1r = cos(arg);
        z1i = -sin(arg);
        z2r = cos(2.*arg);
        z2i = -sin(2.*arg);

        coef = iir->coef;   /* coefficient pointer */
        mag[i] = *coef++;   /* overall K first */

/* loop through all sections: numerator and denominator */
        for(j = 0 ; j < iir->length ; j++) {

/* get the four coefficients for each section */
            beta1 = *coef++;
            beta2 = *coef++;
            alpha1 = *coef++;
            alpha2 = *coef++;

/* denominator */
            d_real = 1 + beta1*z1r + beta2*z2r;
            d_imag = beta1*z1i + beta2*z2i;
            mag[i] = mag[i]/(d_real*d_real + d_imag*d_imag);

/* numerator */
            n_real = 1 + alpha1*z1r + alpha2*z2r;
            n_imag = alpha1*z1i + alpha2*z2i;
            real = n_real*d_real + n_imag*d_imag;
            imag = n_imag*d_real - n_real*d_imag;
            mag[i] = mag[i]*sqrt(real*real + imag*imag);
        }
    }

    return(mag);
}
