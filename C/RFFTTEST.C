#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "dft.h"
#include "get.h"
#include "disk.h"

/*******************************************************************

RFFTTEST.C - Exercise the rfft function for real FFTs

Requires DSP_FILE format time domain input file, Power of 2
length record.  Generates DSP_FILE format spectral magnitude file.

*******************************************************************/

main()
{
  int        i, length, m;
  int        k,view,begin,center;
  float      a,*signal;
  double     tempflt;
  COMPLEX    *out;
  DSP_FILE   *dsp_info;

  dsp_info = open_read(get_string("the input signal file name"));
  length = dsp_info->rec_len;

  m = log2(length);

/* Check for power of 2 input size */

  if((1<<m) != length){
    printf("\n  Input size must be power of 2.\n");
    exit(1);
  }

  signal = read_float_record(dsp_info);

  out = (COMPLEX *) calloc(length/2, sizeof(COMPLEX));
  if(!out){
    printf("\n  Unable to allocate output memory.\n");
    exit(1);
  }

  rfft(signal,out,m);

  a = (float) length*length;
  for (i=0; i<length/2; ++i){
    tempflt  = out[i].real * out[i].real;
    tempflt += out[i].imag * out[i].imag;
    tempflt = tempflt/a;
    out[i].real = 10 * log10(MAX(tempflt,1.e-14)) ;
  }

  center = get_int("center of magnitude file",0,length/2-1);
  view = get_int("length of magnitude file",10,length/2);
  begin = begin = center-view/2;
  
  for(k=0; k<view; k++){
      i = k + begin;
      if (i<0) i = 0;
      if (i>=length/2) i = length/2 - 1;
      signal[k] = out[i].real;
  }

  dsp_info = open_write(get_string("spectral magnitude file name"),
                        FLOAT,1,view);
  
  write_record((char *)signal,dsp_info);

/* make descriptive trailer for magnitude file */
    write_trailer(get_string("trailer string"),dsp_info);
}
