#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "disk.h"
#include "get.h"

/**************************************************************************

STATS.C - PROGRAM TO FIND MIN, MAX, MEAN AND VARIANCE OF RECORDS
          IN AN EXISTING DSP DATA FILE.
          CALLS MINIMUM, MAXIMUM, AVERAGE, AND VARIANCE
          FUNCTIONS CONTAINED IN THIS FILE.

INPUTS: INPUT FILE NAME.

OUTPUTS: STATISTICS OF EACH RECORD

*************************************************************************/

main()
{
    DSP_FILE *dsp_info;
    int i,type,length;
    float var;
    float *signal;
    char *trail;

    float minimum(),maximum(),average(),variance();

    dsp_info = open_read(get_string("input filename"));
    if(!dsp_info) exit(1);     /* bad filename */

    trail = read_trailer(dsp_info);
    if(trail && *trail) printf("\nFile trailer:\n%s\n",trail);

    printf("\n Record   Average   Variance   Sigma      Min       Max");
    printf("\n ------   -------   --------   -----      ---       ---");

    for(i = 0 ; i < dsp_info->records ; i++) {
        signal = read_float_record(dsp_info);

        var = variance(signal,dsp_info->rec_len);

        printf("\n%5d %12.4f %9.4f %9.4f %9.4f %9.4f",
            i,
            average(signal,dsp_info->rec_len),
            var,
            sqrt(var),
            minimum(signal,dsp_info->rec_len),
            maximum(signal,dsp_info->rec_len));

        free((char *)signal);                   /* free after use */
    }
    printf("\n");
}
float minimum(array,size)
    float array[];                        /* function finds min value */
    int size;
{
    int i;
    float ans = array[0];
    for(i = 1 ; i < size ; i++)
        if(array[i] < ans) ans = array[i];
    return(ans);
}
float maximum(array,size)
    float array[];                        /* function finds max value */
    int size;
{
    int i;
    float ans = array[0];
    for(i = 1 ; i < size ; i++)
        if(array[i] > ans) ans = array[i];
    return(ans);
}
float average(array,size)
    float array[];                     /* function calculates average */
    int size;
{
    int i;
    float sum = 0.0;                    /* initialize and declare sum */
    for(i = 0 ; i < size ; i++)
        sum = sum + array[i];               /* calculate sum */
    return(sum/size);                       /* return average */
}
float variance(array,size)            /* function calculates variance */
    float array[];
    int size;
{
    int i;                                 /* declare local variables */
    float ave;
    float sum = 0.0;                      /* initialize sum of signal */
    float sum2 = 0.0;                        /* sum of signal squared */
    for(i = 0 ; i < size ; i++) {
        sum = sum + array[i];
        sum2 = sum2 + array[i]*array[i];       /* calculate both sums */
    }
    ave = sum/size;                              /* calculate average */
    return((sum2 - sum*ave)/(size-1));             /* return variance */
}
