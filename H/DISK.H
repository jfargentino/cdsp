/* disk.h - function prototypes and structures for diskio.c functions */

/* DSP INFORMATION STRUCTURE FOR DSP DATA FILES */

typedef struct {
    unsigned char type;             /* data type 0-7 as defined below */
    unsigned char element_size;     /* size of each element */
    unsigned short int records;     /* number of records */
    unsigned short int rec_len;     /* number of elements in each record */
    char *name;                     /* pointer to file name */
    FILE *fp;                       /* pointer to FILE structure */
} DSP_FILE;

/* defines for data type used in DSP data file header and structure */

#define UNSIGNED_CHAR    0
#define UNSIGNED_INT     1
#define UNSIGNED_LONG    2
#define FLOAT            3
#define SIGNED_CHAR      4
#define SIGNED_INT       5
#define SIGNED_LONG      6
#define DOUBLE           7

/* function prototypes for DSP disk file functions */

    extern DSP_FILE *open_read(char *);
    extern DSP_FILE *open_write(char *,int,int,int);
    extern void read_record(char *,DSP_FILE *);
    extern void write_record(char *,DSP_FILE *);
    extern void seek_record(int,DSP_FILE *);
    extern float *read_float_record(DSP_FILE *);
    extern char *read_trailer(DSP_FILE *);
    extern char *append_trailer(char *,DSP_FILE *);
    extern int write_trailer(char *,DSP_FILE *);
