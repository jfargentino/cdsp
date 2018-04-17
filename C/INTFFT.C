#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "dft.h"
#include "get.h"
#include "disk.h"

/************************************************************************

INTFFT.C - Interpolate 2:1 using FFT and IFFT functions

Requires time domain data file in DSP_FILE format.
Generates 2:1 interpolated time domain data.

*************************************************************************/

main()
{
    int          i, length, fft_length, m;
    float        *signal,*signal2;
    COMPLEX      *samp;
    DSP_FILE     *dsp_info;

/* Read the input data file from the dsp format.  */
    dsp_info = open_read(get_string("the input signal file name"));
    length = dsp_info->rec_len;

    signal = read_float_record(dsp_info);

/* determine fft size */
    m = log2(length);
    fft_length = 1 << m;

/* allocate the complex array (twice as long) */
    samp = (COMPLEX *) calloc(2*fft_length, sizeof(COMPLEX));
    if(!samp) {
        printf("\nError allocating fft memory\n");
        exit(1);
    }

    printf("\nFFT size = %d\n",fft_length);

/* copy input signal to complex array and do the fft */
    for (i = 0; i < length; i++) samp[i].real = signal[i];

    fft(samp,m);

/* divide the middle frequency component by 2 */
    samp[fft_length/2].real = 0.5*samp[fft_length/2].real;
    samp[fft_length/2].imag = 0.5*samp[fft_length/2].imag;

/* zero pad and move the negative frequencies */
    samp[3*fft_length/2] = samp[fft_length/2];
    for (i = fft_length/2 + 1; i < fft_length ; i++) {
        samp[i+fft_length] = samp[i];
        samp[i].real = 0.0;
        samp[i].imag = 0.0;
    }

/* do inverse fft */
    ifft(samp,m+1);

/* allocate the output real samples (twice as long) */
    signal2 = (float *) calloc(2*fft_length, sizeof(float));
    if(!signal2) {
        printf("\nError allocating output memory\n");
        exit(1);
    }

/* copy real part to output and multiply by 2 */  
    for (i=0; i < 2*length; i++) signal2[i] = 2.0*samp[i].real;

/* write out interpolated signal */
    dsp_info = open_write(get_string("output file name"),
                          FLOAT,1,2*length);
  
    write_record((char *)signal2,dsp_info);

/* make descriptive trailer for reconstructed signal file */
    write_trailer(get_string("trailer string"),dsp_info);
}
