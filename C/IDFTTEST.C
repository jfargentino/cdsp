#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "dft.h"
#include "get.h"
#include "disk.h"

/************************************************************

IDFTTEST.C - Demonstrate and test DFT and IDFT functions

Requires and generates DSP_FILE format disk files. 

*************************************************************/

main()
{
  int          i, length;
  float        a,*signal;
  double       tempflt;
  COMPLEX      *samp, *Dataout;
  DSP_FILE     *dsp_info;

/*  Read the input data in from the dsp data file.  */

  dsp_info = open_read(get_string("the input signal file name"));
  length = dsp_info->rec_len;

  signal = read_float_record(dsp_info);

  samp = (COMPLEX *) calloc(length, sizeof(COMPLEX));
  if(!samp){
    printf("\n  Could not allocate sample memory.\n");
    exit(1);
  }

  for (i=0; i<length; i++) samp[i].real = signal[i];

  Dataout = (COMPLEX *) calloc(length, sizeof(COMPLEX));
  if(!Dataout){
    printf("\n  Unable to allocate output memory.\n");
    exit(1);
  }

/*  The dft function prints *'s to let the user know it is
still running. This is necessary because of the relatively
long run time on most machines. */

  printf("  Running DFT...\n");
  dft(samp,Dataout,length);  

/*  Find the magnitude of the Fourier Transform of the sequence.  */

  a = (float) length*length;
  for (i=0; i<length; ++i){
    tempflt  = Dataout[i].real * Dataout[i].real;
    tempflt += Dataout[i].imag * Dataout[i].imag;
    tempflt = tempflt/a;
    signal[i] = 10 * log10(MAX(tempflt,1.e-14));
  }

/*  Write the magnitude out to a data file.  */
  dsp_info = open_write(get_string("spectral magnitude file name"),
                        FLOAT,1,length);

  write_record((char *)signal,dsp_info);

/* make descriptive trailer for magnitude file */
  write_trailer(get_string("trailer string"),dsp_info);

/* Now run the inverse DFT to get the original signal back */

  printf("  Running inverse DFT...\n");
  idft(Dataout,samp,length);

  for (i=0; i<length; ++i) signal[i] = samp[i].real;

/*  Write the reconstructed time domain signal out to a data file. */
  dsp_info = open_write(get_string("reconstructed signal file name"),
                        FLOAT,1,length);
 
  write_record((char *)signal,dsp_info);

/* make descriptive trailer for reconstructed signal file */
  write_trailer(get_string("trailer string"),dsp_info);
}
