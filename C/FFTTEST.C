#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "dft.h"
#include "get.h"
#include "disk.h"

/*********************************************************************

FFTTEST.C - Demonstrate and test FFT and window functions

Accepts time domain input file.  Gives spectral magnitude file
and time domain windowed display as output.

*********************************************************************/

main()
{
  int          i, length, fft_length, m, wnum;
  int          k,view,begin,center;
  float        *signal, *log_mag;
  double       a, tempflt;
  COMPLEX      *samp;
  DSP_FILE     *dsp_info;


/* Read the input signal data file.  */

  dsp_info = open_read(get_string("the input signal file name"));
  length = dsp_info->rec_len;

  signal = read_float_record(dsp_info);

  m = get_int("power of 2 length of FFT",log2(length),12);
  fft_length = 1<<m;

  samp = (COMPLEX *) calloc(fft_length, sizeof(COMPLEX));
  if(!samp){
    printf("\n  Could not allocate sample memory.\n");
    exit(1);
  }

  for (i = 0 ; i < length ; i++) samp[i].real = signal[i];

/*  Get the type of window to use on the time domain data */
  printf("\n  The available window functions are:\n");
  printf("    1 -- Rectangular\n");
  printf("    2 -- Hamming\n");
  printf("    3 -- Hanning\n");
  printf("    4 -- Triangle\n");
  printf("    5 -- Blackman\n");
  printf("    6 -- 4 term Blackman-Harris\n");

  wnum =  get_int("the number of the window function desired",1,6);

/* Perform the window function requested by the user.  */
  switch (wnum) {
  case 1:
    break;
  case 2:
    ham(samp,length);
    break;
  case 3:
    han(samp,length);
    break;
  case 4:
    triang(samp,length);
    break;
  case 5:
    black(samp,length);
    break;
  case 6:
    harris(samp,length);
    break;
           }

/* Write out the windowed signal to a file */
  
  for (i = 0 ; i < length ; i++) signal[i] = samp[i].real;

  dsp_info = open_write(get_string("windowed signal file name"),
                        FLOAT,1,length);
  write_record((char *)signal,dsp_info);

/* make descriptive trailer for windowed signal file */
  write_trailer(get_string("trailer string"),dsp_info);

/* Find the spectrum of the data */
  fft(samp,m);

/* do log magnitude */
  a = 4.0/((double)length * length);
  for (i = 0 ; i < fft_length ; i++) {
    tempflt  = samp[i].real * samp[i].real;
    tempflt += samp[i].imag * samp[i].imag;
    tempflt *= a;
    samp[i].real = 10 * log10(MAX(tempflt,1.e-14));
  }

/*  Copy a section of the spectrum to the magnitude file.
This allows a close up plot of a particular segment of the
spectral magnitude of the signal. */

  center = get_int("center of magnitude file",0,fft_length-1);
  view = get_int("length of magnitude file",10,fft_length);
  begin = center-view/2;
  
  log_mag = (float *) calloc(view,sizeof(float));
  if(!log_mag){
    printf("\n  Unable to allocate log magnitude memory.\n");
    exit(1);
  }

  for(k = 0 ; k < view ; k++){
      i = k + begin;
      if (i<0) i = 0;
      if (i>=fft_length) i = fft_length - 1;
      log_mag[k] = samp[i].real;
  }

  dsp_info = open_write(get_string("spectral magnitude file name"),
                        FLOAT,1,view);
  write_record((char *)log_mag,dsp_info);

/* make descriptive trailer for magnitude file */
  write_trailer(get_string("trailer string"),dsp_info);
}
