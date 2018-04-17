#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "dft.h"
#include "get.h"
#include "disk.h"

/*********************************************************************

PSE.C - Power spectral estimation using the FFT

This program does power spectral estimation on a file with a series
of records.  The average power spectrum in each block is determined
and used to generate a series of PSE records.  The block size
parameters can be selected or a set of defaults can be used.

The default parameters are:

Length of each FFT snapshot: 64 points
Number of FFTs to average: 16 FFTs
Amount of overlap between each FFT: 60 points

*********************************************************************/

main()
{
  int        i, j, m, insize,index;
  int        k,numests,estsize,numav,slice,ovlap;
  float      a,*mag, *sigfloat;
  double     tempflt;
  char       *filnam, trail[100];
  COMPLEX    *samp;
  DSP_FILE   *dsp_info;

  printf("\nThis program does power spectral estimation\n");
  printf("using parameters defined by you or a set of defaults.\n");
  printf("\nThe default parameters are:\n");
  printf("\n  Length of each FFT snapshot:         64 points\n");
  printf("  Number of FFTs to average:           16 FFTs\n");
  printf("  Amount of overlap between each FFT:  60 points\n");


  dsp_info = open_read(get_string("input signal file name"));
  insize = dsp_info->rec_len;

  sigfloat = read_float_record(dsp_info);

  printf("\nUse default parameters or define your own?\n");
  printf("      1 -- Default\n");
  printf("      2 -- Define your own\n");
  if (get_int("your choice",1,2) == 1){
    slice = 64;
    numav = 16;
    ovlap = 60;
  }
  else{
    slice = get_int("length of each fft (must be power of 2)",2,1024);
    numav = get_int("number of spectrums to average",1,256);
    ovlap = get_int("number of overlap points in the FFTs",0,slice-1);
  }
  
/*  Calculate the number of samples in each estimation and
the number of estimations in the data set */

  estsize = (slice-ovlap)*(numav-1) + slice;
  printf("\nThe input file size is: %d\n", insize);
  printf("The size of each estimate is: %d\n", estsize);

  numests = insize/estsize;

  printf("The number of estimates is: %d\n", numests);

  mag = (float *) calloc(slice,sizeof(float));
  samp = (COMPLEX *) calloc(slice,sizeof(COMPLEX));
  if (!mag || !samp){
    printf("\n  Memory allocation error.\n");
    exit(1);
  }

  filnam = get_string("output file name");

  dsp_info = open_write(filnam,FLOAT,numests,slice/2);

  for (i=0; i<numests; i++){

    for (k=0; k<slice; k++) mag[k] = 0;

    for (j=0; j<numav; j++){

      for (k=0; k<slice; k++){
        index = i*estsize + j*(slice-ovlap) + k;
        samp[k].real = sigfloat[index];
        samp[k].imag = 0;
      }

      ham(samp,slice);

      m = log2(slice);
      fft(samp,m);

      a = (float) slice*slice;
      for (k=0; k<slice; k++){
        tempflt  = samp[k].real * samp[k].real;
        tempflt += samp[k].imag * samp[k].imag;
        tempflt = tempflt/a;
        mag[k] += tempflt;
      }
    }

/*  Take log after averaging the magnitudes.  */

    for (k=0; k<slice/2; k++){
      mag[k] = mag[k]/numav;
      mag[k] = 10*log10(MAX(mag[k],1.e-14));
    }

    write_record((char *)mag,dsp_info);

/* Let the user know the program is still running. */
    printf("*");

  }

/* make descriptive trailer for magnitude file */
  sprintf(trail,
    "\n%d power spectral estimates (dB) each of length %d.\n",
    numests, slice);
    
  puts(trail);
  write_trailer(trail,dsp_info);
}
