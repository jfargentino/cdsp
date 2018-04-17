#include <stdlib.h>
#include <stdio.h>
#include "disk.h"
#include "vector.h"

/**************************************************************************

MATIO.C - Routines to read and write matrices

matrix_read      read one matrix
matrix_write     write one matrix

*************************************************************************/

/* MATRIX structure */

typedef struct {
    int element_size;
    unsigned int rows;
    unsigned int cols;
    char **ptr;
               } MATRIX;

/**************************************************************************

matrix_read - open a DSP data file and read it in as a matrix

Open file using file_name and returns pointer to MATRIX structure.
Allocation errors or improper type causes a call to exit(1).
A bad file name returns a NULL pointer.

MATRIX *matrix_read(char *file_name)

*************************************************************************/

MATRIX *matrix_read(file_name)
    char *file_name;
{
    MATRIX *matrix_allocate();
    DSP_FILE *dsp_info;
    MATRIX *A;
    int i,mat_size,length;
    double *buf;          /* input buffer to read data in */

    dsp_info = open_read(file_name);
    if(!dsp_info) return(NULL);     /* bad filename case */

/* determine size of matrix (int, float or double) */
    switch(dsp_info->type) {
        case UNSIGNED_CHAR:
        case SIGNED_CHAR:
        case SIGNED_INT:
            mat_size = sizeof(short int);
            break;
        case UNSIGNED_INT:
        case UNSIGNED_LONG:
        case SIGNED_LONG:
        case FLOAT:
            mat_size = sizeof(float);
            break;
        case DOUBLE:
            mat_size = sizeof(double);
            break;
    }

/* allocate matrix */
    A = matrix_allocate(dsp_info->records,dsp_info->rec_len,mat_size);

/* row length */
    length = dsp_info->rec_len;

/* allocate input buffer area cast to double for worst case alignment */
    buf = (double *) calloc(length,dsp_info->element_size);
    if(!buf) {
        printf("\nBuffer allocation error in matrix_read\n");
        exit(1);
    }

/* read each row and translate */
    for(i = 0 ; i < dsp_info->records ; i++) {

        read_record((char *)buf,dsp_info);

        switch(dsp_info->type) {
            case UNSIGNED_CHAR:
                SCALE_VEC(buf,A->ptr[i],1,length,unsigned char,short int)
                break;
            case SIGNED_CHAR:
                SCALE_VEC(buf,A->ptr[i],1,length,char,short int)
                break;
            case SIGNED_INT:
                SCALE_VEC(buf,A->ptr[i],1,length,short int,short int)
                break;
            case UNSIGNED_INT:
                SCALE_VEC(buf,A->ptr[i],1,length,unsigned short,float)
                break;
            case UNSIGNED_LONG:
                SCALE_VEC(buf,A->ptr[i],1,length,unsigned long,float)
                break;
            case SIGNED_LONG:
                SCALE_VEC(buf,A->ptr[i],1,length,long int,float)
                break;
            case FLOAT:
                SCALE_VEC(buf,A->ptr[i],1,length,float,float)
                break;
            case DOUBLE:
                SCALE_VEC(buf,A->ptr[i],1,length,double,double)
                break;
        }
    }

/* free the dsp info structure and close file since not used again */
    free(dsp_info->name);
    fclose(dsp_info->fp);
    free((char *)dsp_info);

    return(A);
}

/**************************************************************************

matrix_write - open a DSP data file and write out a matrix

Writes matrix of the same type as given in the MATRIX structure
(int, float or double) to file_name.

Returns the DSP_FILE structure for use by other disk I/O
routines such as write_trailer.

Allocation errors or improper type causes a call to exit(1).
Disk open error returns a NULL pointer.

DSP_FILE *matrix_write(MATRIX *A,char *file_name)

*************************************************************************/

DSP_FILE *matrix_write(A,file_name)
    MATRIX *A;
    char *file_name;
{
    DSP_FILE *dsp_info;
    int i,type;

    switch(A->element_size) {
        case sizeof(short int) :
            type = SIGNED_INT;
            break;
        case sizeof(float) :
            type = FLOAT;
            break;
        case sizeof(double) :
            type = DOUBLE;
            break;
        default:
            printf("\nError in MATRIX structure\n");
            exit(1);
    }

    dsp_info = open_write(file_name,type,A->rows,A->cols);
/* if no open return NULL pointer */
    if(!dsp_info) return(NULL);

/* write each row */
    for(i = 0 ; i < dsp_info->records ; i++)
        write_record(A->ptr[i],dsp_info);

    return(dsp_info);
}
