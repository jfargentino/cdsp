#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "dft.h"
#include "get.h"
#include "disk.h"
#include "filter.h"

/***********************************************************************

FASTCON.C - Fast convolution using the FFT

This program performs fast convolution using the FFT.  It performs
the convolution required to implement a 35 point FIR filter
(store in variable fir_lpf35, defined in filter.h) on an
arbitrary length input file specified by the user.  The filter is
a LPF with 40 dB out of band rejection.  The 3 dB point is at a
relative frequency of approximately .25*fs.

************************************************************************/

main()
{
  int          i, length, m, fft_length;
  float        a,*signal;
  double       tempflt;
  COMPLEX      *samp, *filt;
  DSP_FILE     *dsp_info;
  char         *trail;

/* Read the input data file specified by the user */

  dsp_info = open_read(get_string("input signal file name"));
  length = dsp_info->rec_len;

  signal = read_float_record(dsp_info);

  m = log2(length);
  samp = (COMPLEX *) calloc(fft_length=1<<m, sizeof(COMPLEX));
  if(!samp){
    printf("\n  Could not allocate sample memory.\n");
    exit(1);
  }

  for (i = 0; i < fft_length; i++) samp[i].real = signal[i];

/* Let the user know that the FFT processing has started */

  fft(samp,m);

/* Place the log magnitude of the FFT in a dsp data file */
  a = (float) length*length;
  for (i = 0; i < length; i++) {
    tempflt  = samp[i].real * samp[i].real;
    tempflt += samp[i].imag * samp[i].imag;
    tempflt = tempflt/a;
    signal[i] = 10 * log10(MAX(tempflt,1.e-14)) ;
  }

  dsp_info = open_write(
             get_string("original signal spectral magnitude file name"),
                        FLOAT,1,length);
  
  write_record((char *)signal,dsp_info);

/* make descriptive trailer for magnitude file */
  write_trailer(get_string("trailer string"),dsp_info);

/* Zero fill the filter to the sequence length */
  filt = (COMPLEX *) calloc(fft_length, sizeof(COMPLEX));
  if(!filt){
    printf("\n  Could not allocate filter memory.\n");
    exit(1);
  }
  
  for (i = 0; i < 35; i++) filt[i].real = fir_lpf35[i];

/* FFT the zero filled filter impulse response */

  fft(filt,m);

  a = 35*35;        /* Scale the filter magnitude to filter length */

  for (i = 0; i < length; i++) {
    tempflt  = filt[i].real * filt[i].real;
    tempflt += filt[i].imag * filt[i].imag;
    tempflt = tempflt/a;
    signal[i] = 10 * log10(MAX(tempflt,1e-14));
  }

/* Write filter magnitude transfer function to file */ 

  dsp_info = open_write(
             get_string("filter spectral magnitude file name"),
                        FLOAT,1,length);
  
  write_record((char *)signal,dsp_info);

/* make descriptive trailer for magnitude file */
  trail = "\nSpectrum of impulse response of 35 tap FIR filter.\n";
  write_trailer(trail,dsp_info);

/* Multiply the two transformed sequences */

  for (i = 0; i < length; i++) {
    tempflt = samp[i].real * filt[i].real
                           - samp[i].imag * filt[i].imag;
    samp[i].imag = samp[i].real * filt[i].imag
                           + samp[i].imag * filt[i].real;
    samp[i].real = tempflt;
  }

/* Inverse fft the multiplied sequences */

  ifft(samp,m);

/* Write the result out to a dsp data file */
  for (i = 0; i < length; i++) signal[i] = samp[i].real;
  dsp_info = open_write(get_string("filter output signal file name"),
                        FLOAT,1,length);

  write_record((char *)signal,dsp_info);

/* make descriptive trailer for reconstructed signal file */
  write_trailer(get_string("trailer string"),dsp_info);
}
