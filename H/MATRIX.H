#include "disk.h"       /* sometimes need diskio */

/* header file for matrix function and MATRIX structure */

typedef struct {
    int element_size;
    unsigned int rows;
    unsigned int cols;
    char **ptr;
               } MATRIX;

    extern MATRIX *matrix_allocate(int,int,int);
    extern void matrix_free(MATRIX *);
    extern void matrix_print(MATRIX *);
    extern MATRIX *matrix_scale(MATRIX *,double);
    extern MATRIX *matrix_crop(MATRIX *,int,int,int,int);
    extern MATRIX *matrix_add(MATRIX *,MATRIX *);
    extern MATRIX *matrix_sub(MATRIX *,MATRIX *);
    extern MATRIX *matrix_mult_pwise(MATRIX *,MATRIX *);
    extern MATRIX *matrix_mult(MATRIX *,MATRIX *);
    extern MATRIX *matrix_transpose(MATRIX *);
    extern MATRIX *matrix_invert(MATRIX *);
    extern double matrix_det(MATRIX *);
    extern MATRIX *matrix_read(char *);
    extern DSP_FILE *matrix_write(MATRIX *,char *);

/* image processing routines */
    extern void dct2d(MATRIX *);
    extern void idct2d(MATRIX *);
    extern float *histogram(MATRIX *,int,int);
    extern MATRIX *convol2d(MATRIX *,MATRIX *);
    extern MATRIX *nonlin2d(MATRIX *,int,int);
