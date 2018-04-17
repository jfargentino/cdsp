#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "dft.h"
#include "get.h"
#include "disk.h"

/************************************************************************

IFFTTEST.C - Demonstrate and test FFT and IFFT functions

Requires time domain data file in DSP_FILE format.
Generates DSP_FILE format files for spectral magnitude
and reconstructed time domain data.

*************************************************************************/

main()
{
    int          i, length, fft_length, m;
    double       a, tempflt;
    float        *signal,*log_mag;
    COMPLEX      *samp;
    DSP_FILE     *dsp_info;

/* Read the input data file from the dsp format.  */
    dsp_info = open_read(get_string("the input signal file name"));
    length = dsp_info->rec_len;

    signal = read_float_record(dsp_info);

/* determine fft size and allocate the complex array */
    m = log2(length);
    fft_length = 1 << m;

    samp = (COMPLEX *) calloc(fft_length, sizeof(COMPLEX));
    if(!samp) {
        printf("\nError allocating fft memory\n");
        exit(1);
    }

    printf("\nFFT size = %d\n",fft_length);

/* copy input signal to complex array and do the fft */
    for (i=0; i<length; i++) samp[i].real = signal[i];

    fft(samp,m);

/* find log magnitude and store for output */
    log_mag = (float *)calloc(fft_length,sizeof(float));
    if(!log_mag) {
        printf("\nError allocating magnitude memory\n");
        exit(1);
    }

    a = (float) length * length;
    for (i=0; i< fft_length; i++) {
        tempflt  = samp[i].real * samp[i].real;
        tempflt += samp[i].imag * samp[i].imag;
        tempflt = tempflt/a;
        log_mag[i] = 10 * log10(MAX(tempflt,1.e-14));
    }

    dsp_info = open_write(get_string("spectral magnitude file name"),
                          FLOAT,1,fft_length);
  
    write_record((char *)log_mag,dsp_info);

/* make descriptive trailer for magnitude file */
    write_trailer(get_string("trailer string"),dsp_info);

/* do inverse fft and write out reconstructed signal */

    ifft(samp,m);
  
    for (i=0; i<length; ++i) signal[i] = samp[i].real;

    dsp_info = open_write(get_string("reconstructed signal file name"),
                          FLOAT,1,length);
  
    write_record((char *)signal,dsp_info);

/* make descriptive trailer for reconstructed signal file */
    write_trailer(get_string("trailer string"),dsp_info);
}
