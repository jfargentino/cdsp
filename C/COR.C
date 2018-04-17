#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vector.h"
#include "disk.h"
#include "get.h"

/**************************************************************************

COR.C - PERFORM CROSS CORRELATION OR AUTO CORRELATION

INPUTS: TWO FILE NAMES OF THE DSP DATA RECORDS TO BE CORRELATED,
        OFFSETS, CORRELATION LENGTH, MIN AND MAX LAG VALUE.

OUTPUTS: CORRELATION SUMS FOR EACH LAG IN ONE RECORD OF A DSP DATA FILE.

*************************************************************************/

main()
{
    DSP_FILE *dsp_x,*dsp_y,*dsp_out;  /* input and output files */

    int i,d,xoffset,yoffset,cor_length,min_lag,max_lag,lag_limit,out_len;
    float sum;
    float *x,*y;          /* pointers to input signals */
    float *cor_output;

    char *file;
    static char trailer[300];

/* get X signal and file name */
    file = get_string("file name for X signal");
    dsp_x = open_read(file);
    if(!dsp_x) exit(1);     /* bad filename */
    x = read_float_record(dsp_x); /* read first record */

/* get Y signal and file name */
    file = get_string("file name for Y signal");
    dsp_y = open_read(file);
    if(!dsp_y) exit(1);     /* bad filename */
    y = read_float_record(dsp_y); /* read first record */

/* x and y signal offsets for start of correlation */
    xoffset = get_int("X signal offset",0,dsp_x->rec_len - 1);
    yoffset = get_int("Y signal offset",0,dsp_y->rec_len - 1);

/* get correlation length (within limits) */
    cor_length = get_int("correlation length",1,
        MIN(dsp_x->rec_len - xoffset,dsp_y->rec_len - yoffset));

/* lag range to use */
    lag_limit = dsp_y->rec_len - yoffset - cor_length;
    min_lag = get_int("minimum lag to calculate",-yoffset,lag_limit);
    max_lag = get_int("maximum lag to calculate",min_lag,lag_limit);

/* allocate result array */
    out_len = max_lag - min_lag + 1;
    cor_output = (float *)calloc(out_len,sizeof(float));
    if(!cor_output) {
        printf("\nUnable to allocate output array\n");
        exit(1);
    }

/* perform correlation */
    for(i = 0 , d = min_lag ; d <= max_lag ; d++ , i++) {
        DOTP_VEC(x + xoffset, y + yoffset + d,sum,cor_length,float,float)
        cor_output[i] = sum;
    }

/* open output file */
    dsp_out = open_write(get_string("output file name"),FLOAT,1,out_len);
    if(!dsp_out) exit(1);

/* write out correlation results */
    write_record((char *)cor_output,dsp_out);

/* make trailer and write to output file */
    sprintf(trailer,"File %s correlated with file %s\n",
        dsp_x->name,dsp_y->name);
    sprintf(strchr(trailer,0),"X offset = %d Y offset = %d\n",
        xoffset,yoffset);
    sprintf(strchr(trailer,0),
        "Correlation length = %d Minimum lag = %d Maximum lag = %d\n",
        cor_length,min_lag,max_lag);

    write_trailer(trailer,dsp_out);
}
