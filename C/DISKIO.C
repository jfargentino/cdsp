#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**************************************************************************

DISKIO.C - Source code for DSP data format read and write functions

    open_read          open DSP data file to be read
    open_write         create header and open DSP data file for write
    read_record        read one record
    write_record       write one record
    seek_record        seek to beginning of specified record
    read_float_record  read one record and convert to float array
    read_trailer       read the trailer text
    write_trailer      write the trailer text
    append_trailer     read a trailer and add to it

*************************************************************************/

/* DSP INFORMATION STRUCTURE FOR MANIPULATING DSP DATA FILES */
typedef struct {
    unsigned char type;             /* data type 0-7 as defined below */
    unsigned char element_size;     /* size of each element */
    unsigned short int records;     /* number of records */
    unsigned short int rec_len;     /* number of elements in each record */
    char *name;                     /* pointer to file name */
    FILE *fp;                       /* pointer to FILE structure */
} DSP_FILE;

/* FILE HEADER STRUCTURE FOR DSP DATA FILES */
typedef struct {
    unsigned char type;             /* data type 0-7 as defined below */
    unsigned char element_size;     /* size of each element */
    unsigned short int records;     /* number of records */
    unsigned short int rec_len;     /* number of elements in each record */
} HEADER;

/* defines for data type used ind DSP data file header and structure */

#define UNSIGNED_CHAR    0
#define UNSIGNED_INT     1
#define UNSIGNED_LONG    2
#define FLOAT            3
#define SIGNED_CHAR      4
#define SIGNED_INT       5
#define SIGNED_LONG      6
#define DOUBLE           7

/**************************************************************************

open_read - open a DSP data file for read

Returns a pointer to a DSP_FILE structure allocated by the
function and opens file_name.

Allocation errors or improper type causes a call to exit(1).
A bad file_name returns a NULL pointer.

DSP_FILE *open_read(char *file_name)

*************************************************************************/

DSP_FILE *open_read(file_name)
    char *file_name;            /* file name string */
{
    DSP_FILE *dsp_info;
    int status;

/* allocate the DSP data file structure */

    dsp_info = (DSP_FILE *) malloc(sizeof(DSP_FILE));
    if(!dsp_info) {
        printf("\nError in open_read: structure allocation, file %s\n",
                    file_name);
        exit(1);
    }

/* open file for binary read and update */
    dsp_info->fp = fopen(file_name,"r+b");
    if(!dsp_info->fp) {
        printf("\nError opening %s in open_read\n",file_name);
        return(NULL);
    }

/* copy and allocate file name string for the DSP_FILE structure */
    dsp_info->name = malloc(strlen(file_name) + 1);
    if(!dsp_info->name) {
        printf("\nUnable to allocate file_name string in open_read\n");
        exit(1);
    }
    strcpy(dsp_info->name,file_name);

/* read in header from file */
    status = fread((char *)dsp_info,sizeof(HEADER),1,dsp_info->fp);
    if(status != 1) {
        printf("\nError reading header of file %s\n",file_name);
        exit(1);
    }

/* return pointer to DSP_FILE structure */
    return(dsp_info);
}

/**************************************************************************

open_write - open a DSP data file for write

Returns a pointer to a DSP_FILE structure allocated by the function.
Allocation errors or improper type causes a call to exit(1).
A bad file name returns a NULL pointer.

DSP_FILE *open_write(char *file_name,int type,int records,int rec_len)

    file_name       pointer to file name string
    type            type of DSP data (0-7 specified in defines)
    records         number of records of data to be written
    rec_len         number of elements in each record

*************************************************************************/

DSP_FILE *open_write(file_name,type,records,rec_len)
    char *file_name;              /* file name string */
    int type;                     /* data type 0-7 */
    unsigned short int records;   /* number of records to be written */
    unsigned short int rec_len;   /* elements in each record */
{
    DSP_FILE *dsp_info;
    int status;

/* allocate the DSP data file structure */
    dsp_info = (DSP_FILE *) malloc(sizeof(DSP_FILE));
    if(!dsp_info) {
        printf("\nError in open_write: structure allocation, file %s\n",
                    file_name);
        exit(1);
    }

/* set the basics */
    dsp_info->type = (unsigned char)type;
    dsp_info->records = records;
    dsp_info->rec_len = rec_len;

/* set element size from data type */
    switch(type) {
        case 0:
        case 4:
            dsp_info->element_size = sizeof(char);
            break;
        case 1:
        case 5:
            dsp_info->element_size = sizeof(short int);
            break;
        case 2:
        case 6:
            dsp_info->element_size = sizeof(long int);
            break;
        case 3:
            dsp_info->element_size = sizeof(float);
            break;
        case 7:
            dsp_info->element_size = sizeof(double);
            break;
        default:
            printf("\nUnsupported data type, file %s\n",file_name);
            exit(1);
    }

/* open file for binary write */
    dsp_info->fp = fopen(file_name,"wb");
    if(!dsp_info->fp) {
        printf("\nError opening %s in open_write\n",file_name);
        return(NULL);
    }

/* copy and allocate file name string for the DSP_FILE structure */
    dsp_info->name = malloc(strlen(file_name) + 1);
    if(!dsp_info->name) {
        printf("\nUnable to allocate file_name string in open_write\n");
        exit(1);
    }
    strcpy(dsp_info->name,file_name);

/* write header to file */
    status = fwrite((char *)dsp_info,sizeof(HEADER),1,dsp_info->fp);
    if(status != 1) {
        printf("\nError writing header of file %s\n",file_name);
        exit(1);
    }

/* return pointer to DSP_FILE structure */
    return(dsp_info);
}

/**************************************************************************

read_record - read one record of DSP data file

Exits if a read error occurs or if the DSP_FILE structure is invalid.
    
void read_record(char *ptr,DSP_FILE *dsp_info)

    ptr        pointer to previously allocated memory to put data
    dsp_info   pointer to DSP data file structure

*************************************************************************/

void read_record(ptr,dsp_info)
    char *ptr;                  /* pointer to some type of data */
    DSP_FILE *dsp_info;
{
    int status;

    if(!dsp_info) {
        printf("\nError in DSP_FILE structure passed to read_record\n");
        exit(1);
    }

    status = fread(ptr,dsp_info->element_size,
                            dsp_info->rec_len,dsp_info->fp);
    if(status != dsp_info->rec_len) {
        printf("\nError in read_record, file %s\n",dsp_info->name);
        exit(1);
    }
}

/**************************************************************************

write_record - write one record of DSP_FILE data

Exits if write error occurs or if the DSP_FILE structure is invalid.

void write_record(char *ptr,DSP_FILE *dsp_info)

    ptr        pointer to data to write to disk (type in dsp_info)
    dsp_info   pointer to DSP data file structure

*************************************************************************/

void write_record(ptr,dsp_info)
    char *ptr;                  /* pointer to some type of data */
    DSP_FILE *dsp_info;
{
    int status;

    if(!dsp_info) {
        printf("\nError in DSP_FILE structure passed to write_record\n");
        exit(1);
    }

    status = fwrite(ptr,dsp_info->element_size,
                             dsp_info->rec_len,dsp_info->fp);
    if(status != dsp_info->rec_len) {
        printf("\nError write_record, file %s\n",dsp_info->name);
        exit(1);
    }
}

/**************************************************************************

seek_record - seek to the beginning of a record of DSP data file

If improper seek is requested, the function calls exit(1).

void seek_record(int record_num,DSP_FILE *dsp_info)

    record_num    integer record number to seek in DSP data file
    dsp_info      pointer to DSP data file structure

*************************************************************************/

void seek_record(record_num,dsp_info)
    int record_num;                  /* record number to seek */
    DSP_FILE *dsp_info;
{
    long int position,bytecount;

    if(!dsp_info) {
        printf("\nError in DSP_FILE structure passed to seek_record\n");
        exit(1);
    }

/* get the number of bytes to the beginning of the record of data */
    bytecount = (long)dsp_info->element_size * (long)dsp_info->rec_len
                                             * (long)record_num;
    bytecount += sizeof(HEADER);   /* add on the header length */

/* find the end of file location */
    fseek(dsp_info->fp,0L,2);       /* seek to end of file */
    position = ftell(dsp_info->fp);

/* check for errors in position */
    if(position <= 0L || position < bytecount) {
        printf("\nError in locating record in file %s\n",dsp_info->name);
        exit(1);
    }
    fseek(dsp_info->fp,bytecount,0);         /* move to record */
}

/**************************************************************************

read_float_record - read one record of DSP data file and convert
                    to float array of values.

Returns a pointer to the beginning of the allocated float array
of values representing the record read from the DSP_FILE.

Exits if a read or allocation error occurs.

float *read_float_record(DSP_FILE *dsp_info)

*************************************************************************/

float *read_float_record(dsp_info)
    DSP_FILE *dsp_info;
{
    void read_record();
    static long int prev_size = 0; /* previous size in bytes */
    static double *buf;            /* input buffer to read data in */

    float *out;                    /* return output pointer */
    float *out_ptr;

    long int byte_size;            /* current size in bytes */
    int i,length;

    length = dsp_info->rec_len;

    byte_size = (long)length*dsp_info->element_size;

/* check to see if we have to allocate the input buffer */
    if(byte_size != prev_size) {

        if(prev_size != 0) free(buf);   /* free old buffer */

/* allocate input buffer area cast to double for worst case alignment */
        buf = (double *) calloc(length,dsp_info->element_size);

        if(!buf) {
            printf("\nAllocation error in input buffer\n");
            exit(1);
        }

        prev_size = byte_size;      /* lattest size */
    }

/* allocate the output pointer only if conversion required */
    if(dsp_info->type != FLOAT) {
        out = (float *) calloc(length,sizeof(float));
        if(!out) {
            printf("\nAllocation error in read_float_record\n");
            exit(1);
        }
    }

/* read the record into buf */
    read_record((char *)buf,dsp_info);

/* perform conversion to floating point */

    out_ptr = out;

    switch(dsp_info->type) {
        case UNSIGNED_CHAR: {
                unsigned char *uc_ptr;
                uc_ptr = (unsigned char *)buf;
                for(i = 0 ; i < length ; i++)
                    *out_ptr++ = (float)(*uc_ptr++);
            }
            break;
        case SIGNED_CHAR: {
                char *sc_ptr;
                sc_ptr = (char *)buf;
                for(i = 0 ; i < length ; i++)
                    *out_ptr++ = (float)(*sc_ptr++);
            }
            break;
        case SIGNED_INT: {
                int *si_ptr;
                si_ptr = (int *)buf;
                for(i = 0 ; i < length ; i++)
                    *out_ptr++ = (float)(*si_ptr++);
            }
            break;
        case UNSIGNED_INT: {
                unsigned int *ui_ptr;
                ui_ptr = (unsigned int *)buf;
                for(i = 0 ; i < length ; i++)
                    *out_ptr++ = (float)(*ui_ptr++);
            }
            break;
        case UNSIGNED_LONG: {
                unsigned long *ul_ptr;
                ul_ptr = (unsigned long *)buf;
                for(i = 0 ; i < length ; i++)
                    *out_ptr++ = (float)(*ul_ptr++);
            }
            break;
        case SIGNED_LONG: {
                long *sl_ptr;
                sl_ptr = (long *)buf;
                for(i = 0 ; i < length ; i++)
                    *out_ptr++ = (float)(*sl_ptr++);
            }
            break;
        case FLOAT:
            out = (float *) buf;        /* no conversion */
            prev_size = 0;              /* force next allocation */
            break;
        case DOUBLE: {
                double *d_ptr;
                d_ptr = buf;
                for(i = 0 ; i < length ; i++)
                    *out_ptr++ = (float)(*d_ptr++);
            }
            break;
    }

    return(out);                 /* return converted pointer */
}

/**************************************************************************

read_trailer - read trailer from existing DSP data file

Returns pointer to allocated string.
Returns NULL if read error or file error.
Exits if allocation error.

char *read_trailer(DSP_FILE *dsp_info)

*************************************************************************/

char *read_trailer(dsp_info)
    DSP_FILE *dsp_info;
{
    int status,len;
    long int position,bytecount,old_pos;
    char *text;

/* get the number of bytes in the data */
    bytecount = (long)dsp_info->element_size * (long)dsp_info->records
                                             * (long)dsp_info->rec_len;
    bytecount += sizeof(HEADER);   /* add on the header length */

/* save current position */
    old_pos = ftell(dsp_info->fp);

/* find the end of file location */
    fseek(dsp_info->fp,0L,2);       /* seek to end of file */
    position = ftell(dsp_info->fp);

/* check for errors in position */
    if(position <= 0L || position < bytecount) {
        printf("\nError in trailer, file %s\n",dsp_info->name);
        return(NULL);
    }

/* try to allocate space for the trailer text */
    len = (int)(position - bytecount);
    text = malloc(len+1);       /* +1 for NULL termination */
    if(!text) {
        printf("\nError in allocating trailer space for file %s\n",
                dsp_info->name);
        exit(1);
    }

/* read trailer */
    fseek(dsp_info->fp,bytecount,0);    /* seek to beginning of trailer */
    status = fread(text,sizeof(char),len,dsp_info->fp);
    if(status != len) {
        printf("\nError reading trailer of file %s\n",dsp_info->name);
        return(NULL);
    }

/* restore file position */
    fseek(dsp_info->fp,old_pos,0);

/* make sure the trailer is NULL terminated */
    text[len] = NULL;

    return(text);
}

/**************************************************************************

write_trailer - write trailer to DSP data file

Writes *text to the trailer of the DSP_FILE.
Exits with error message if write or positioning error.
Returns the number of characters in the trailer string.

int write_trailer(char *text,DSP_FILE *dsp_info)

*************************************************************************/

int write_trailer(text,dsp_info)
    char *text;             /* pointer to trailer text */
    DSP_FILE *dsp_info;
{
    int status,len;
    long int position,bytecount,old_pos;

/* get the number of bytes in the data */
    bytecount = (long)dsp_info->element_size * (long)dsp_info->records
                                             * (long)dsp_info->rec_len;
    bytecount += sizeof(HEADER);   /* add on the header length */

/* save current position */
    old_pos = ftell(dsp_info->fp);

    fseek(dsp_info->fp,bytecount,0);    /* seek to beginning of trailer */

/* determine current file position and check if all records written */
    position = ftell(dsp_info->fp);

    if(bytecount != position || position <= 0L) {
        printf("\nError in write_trailer: all records not written, file %s\n",
                    dsp_info->name);
        exit(1);
    }

/* write out the trailer */
    len = strlen(text);

    status = fwrite(text,sizeof(char),len,dsp_info->fp);
    if(status != len) {
        printf("\nError in writing trailer to file %s\n",dsp_info->name);
        exit(1);
    }

/* restore file position */
    fseek(dsp_info->fp,old_pos,0);

    return(len);        /* return length of trailer */
}

/**************************************************************************

append_trailer - read trailer from existing DSP data file
                 and add on a new string

Returns pointer to new trailer which is the old trailer + *string.
Exits if read error, file error, or allocation error.

char *append_trailer(char *string, DSP_FILE *dsp_info)

*************************************************************************/

char *append_trailer(string,dsp_info)
    char *string;           /* string to add */
    DSP_FILE *dsp_info;     /* input trailer file */
{
    char *read_trailer();
    char *trail;            /* trailer pointer */

    trail = read_trailer(dsp_info);
    
/* re-allocate the output for the total size */

    trail = realloc(trail,strlen(trail) + strlen(string) + 1);
    if(!trail) {
        printf("\nError in re-allocating trailer space for file %s\n",
                dsp_info->name);
        exit(1);
    }

/*  add on string to trail */

    strcat(trail,string);

    return(trail);
}
