#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* structure used by all matrix routines */

typedef struct {
    int element_size;
    unsigned int rows;
    unsigned int cols;
    char **ptr;
               } MATRIX;

/**************************************************************************

MATRIX.C - Source code for matrix functions

matrix_allocate   allocate matrix, return pointer to MATRIX structure
matrix_free       free matrix area and MATRIX structure
matrix_print      print the elements of a matrix to stdout
matrix_scale      scale all of a matrix by s
matrix_crop       form sub-matrix from a larger matrix
matrix_transpose  transpose a matrix, return pointer to new MATRIX
matrix_add        add two matrices and return a sum MATRIX
matrix_sub        subtract two matrices and return a difference MATRIX
matrix_mult_pwise multiply two matrices element by element
matrix_mult       multiply two matrices, return new MATRIX
matrix_invert     invert a square matrix, return inverse MATRIX
matrix_det        return the determinant of a square matrix

*************************************************************************/

/**************************************************************************

matrix_allocate - Make matrix of given size (rows x cols) and type

The type is given by element_size (2 = ints, 4 = floats, 8 = doubles).
Exits if the matrix could not be allocated.

    MATRIX *matrix_allocate(int rows,int cols,int element_size)

*************************************************************************/

MATRIX *matrix_allocate(rows,cols,element_size)
    int rows,cols,element_size;
{
    int i;
    MATRIX *A;

/* allocate the matrix structure */
    A = (MATRIX *) calloc(1,sizeof(MATRIX));
    if(!A) {
        printf("\nERROR in matrix structure allocate\n");
        exit(1);
    }

/* set up the size as requested */
    A->rows = rows;
    A->cols = cols;
    A->element_size = element_size;

/* try to allocate the request */
    switch(element_size) {
        case sizeof(short): {    /* integer matrix */
            short **int_matrix;
            int_matrix = (short **)calloc(rows,sizeof(short *));
            if(!int_matrix) {
                printf("\nError making pointers in %dx%d int matrix\n"
                            ,rows,cols);
                exit(1);
            }
            for(i = 0 ; i < rows ; i++) {
                int_matrix[i] = (short *)calloc(cols,sizeof(short));
                if(!int_matrix[i]) {
                    printf("\nError making row %d in %dx%d int matrix\n"
                            ,i,rows,cols);
                    exit(1);
                }
            }
            A->ptr = (char **)int_matrix;
            break;
        }
        case sizeof(float): {    /* float matrix */
            float **float_matrix;
            float_matrix = (float **)calloc(rows,sizeof(float *));
            if(!float_matrix) {
                printf("\nError making pointers in %dx%d float matrix\n"
                            ,rows,cols);
                exit(1);
            }
            for(i = 0 ; i < rows ; i++) {
                float_matrix[i] = (float *)calloc(cols,sizeof(float));
                if(!float_matrix[i]) {
                    printf("\nError making row %d in %dx%d float matrix\n"
                            ,i,rows,cols);
                    exit(1);
                }
            }
            A->ptr = (char **)float_matrix;
            break;
        }
        case sizeof(double): {   /* double matrix */
            double **double_matrix;
            double_matrix = (double **)calloc(rows,sizeof(double *));
            if(!double_matrix) {
                printf("\nError making pointers in %dx%d double matrix\n"
                            ,rows,cols);
                exit(1);
            }
            for(i = 0 ; i < rows ; i++) {
                double_matrix[i] = (double *)calloc(cols,sizeof(double));
                if(!double_matrix[i]) {
                    printf("\nError making row %d in %dx%d double matrix\n"
                            ,i,rows,cols);
                    exit(1);
                }
            }
            A->ptr = (char **)double_matrix;
            break;
        }
        default:
            printf("\nERROR in matrix_allocate: unsupported type\n");
            exit(1);
    }
    return(A);
}

/**************************************************************************

matrix_free - Free all elements of matrix

Frees the matrix data (rows and cols), the matrix pointers and
the MATRIX structure.

Error message and exit if improper structure is
passed to it (null pointers or zero size matrix).

    void matrix_free(MATRIX *A)

*************************************************************************/

void matrix_free(A)
    MATRIX *A;
{
    int i;
    char **a;

/* check for valid structure */
    if(!A || !A->ptr || !A->rows || !A->cols) {
        printf("\nERROR: invalid structure passed to matrix_free");
        exit(1);
    }

/* a used for freeing */
    a = A->ptr;

/* free each row of data */
    for(i = 0 ; i < A->rows ; i++) free(a[i]);

/* free each row pointer */
    free((char *)a);
    a = NULL;           /* set to null for error */

/* free matrix structure */
    free((char *)A);
}

/**************************************************************************

matrix_print - Display matrix on display device

Uses printf to display elements using %g or %d format.

    void matrix_print(MATRIX *A)

*************************************************************************/

void matrix_print(A)
    MATRIX *A;
{
    int i,j,cols_lin;

    if(A->element_size == sizeof(short))
        cols_lin = 10;       /* 10 per line for int */
    else
        cols_lin = 5;        /* 5 per line for float types */

    for(i = 0 ; i < A->rows ; i++) {
        for(j = 0 ; j < A->cols ; j++) {
            if(j%cols_lin == 0) {    /* newline every cols_lin */
                if(j == 0)           /* start of row */
                    printf("\nRow%3d:",i);
                else
                    printf("\n       ");
            }
            switch(A->element_size) {
                case sizeof(short):
                    printf("%7d",((short **)A->ptr)[i][j]);
                    break;
                case sizeof(float):
                    printf("%14.5g",((float **)A->ptr)[i][j]);
                    break;
                case sizeof(double):
                    printf("%14.5lg",((double **)A->ptr)[i][j]);
            }
        }
    }
}

/**************************************************************************

matrix_scale - Matrix multiply by a scalar, sA

Returns the new scaled matrix as a pointer to a new matrix structure.

The scalar s is a double constant.  Integer matrices are promoted to
floats when scaled.  Uses SCALE_MAT macro for the three cases.

    MATRIX *matrix_scale(MATRIX *A,double s)

*************************************************************************/

/*
SCALE_MAT MACRO FOR matrix_scale FUNCTION:

SCALE_MAT(a,b,s,rows,cols,rowoff,coloff,typea,typeb)

    a       pointer to input MATRIX structure.
    b       pointer to resultant MATRIX structure.
    s       scale factor variable to be multiplied.
    rows    number of rows in matrix b
    cols    number of columns in matrix b
    rowoff  number of rows to offset matrix b
    coloff  number of columns to offset matrix b
    typea   legal C type describing the type of a
    typeb   legal C type describing the type of b

*/

#define SCALE_MAT(a,b,s,rows,cols,rowoff,coloff,typea,typeb) {  \
                 typea **_AMX = (typea **)a->ptr;  \
                 typeb **_BMX = (typeb **)b->ptr;  \
                 typea *_PTA;  \
                 typeb *_PTB;  \
                 int _IX,_JX;  \
                 for(_IX = 0 ; _IX < rows ; _IX++) {  \
                     _PTB = _BMX[_IX];  \
                     _PTA = _AMX[_IX + rowoff] + coloff;  \
                     for(_JX = 0 ; _JX < cols ; _JX++)  \
                         *_PTB++ = (typeb)(s * (*_PTA++));  \
                 }  \
             }    

MATRIX *matrix_scale(a,s)
    MATRIX *a;
    double s;
{
    MATRIX *matrix_allocate();
    MATRIX *b;
    int a_r,a_c;

/* simplify the row and col variables */
    a_r = a->rows;
    a_c = a->cols;

/* do the different type cases with a switch statement */
    switch(a->element_size) {
        case sizeof(short):    /* if a int, b is float */
            b = matrix_allocate(a_r,a_c,sizeof(float));
            SCALE_MAT(a,b,s,a_r,a_c,0,0,short,float)
            break;
        case sizeof(float):    /* a float, b float */
            b = matrix_allocate(a_r,a_c,sizeof(float));
            SCALE_MAT(a,b,s,a_r,a_c,0,0,float,float)
            break;
        case sizeof(double):    /* a double, b double */
            b = matrix_allocate(a_r,a_c,sizeof(double));
            SCALE_MAT(a,b,s,a_r,a_c,0,0,double,double)
    }
    return(b);
}

/**************************************************************************

matrix_crop - Forms sub-matrix of given size and location from original

Returns the new smaller matrix as a pointer to
a new MATRIX structure using the row and col offsets
and number of rows and cols in the result matrix.

    MATRIX *matrix_crop(MATRIX *A,
                  int row_offset,int col_offset,int rows,int cols);

*************************************************************************/

MATRIX *matrix_crop(a,row_offset,col_offset,rows,cols)
    MATRIX *a;
    int row_offset,col_offset,rows,cols;
{
    MATRIX *matrix_allocate();
    MATRIX *b;

/* check the row and col variables */
    if((rows + row_offset) > a->rows || (cols + col_offset) > a->cols) {
        printf("\nERROR in matrix_crop: sub-matrix exceeds matrix size\n");
        exit(1);
    }

/* allocate the output sub-matrix */
    b = matrix_allocate(rows,cols,a->element_size);

/* do the different type cases with a switch statement */
    switch(a->element_size) {
        case sizeof(short):
            SCALE_MAT(a,b,1,rows,cols,row_offset,col_offset,short,short)
            break;
        case sizeof(float):
            SCALE_MAT(a,b,1,rows,cols,row_offset,col_offset,float,float)
            break;
        case sizeof(double):
            SCALE_MAT(a,b,1,rows,cols,row_offset,col_offset,double,double)
    }
    return(b);
}

/**************************************************************************

matrix_transpose - Transpose matrix by copying to a new matrix

    MATRIX *matrix_transpose(MATRIX *A);

*************************************************************************/

MATRIX *matrix_transpose(A)
    MATRIX *A;
{
    MATRIX *matrix_allocate();
    MATRIX *At;
    short **ai,**ait;
    float **af,**aft;
    double **ad,**adt;
    int i,j;

/* allocate transposed output space of same type */
    At = matrix_allocate(A->cols,A->rows,A->element_size);

    switch(A->element_size) {
        case sizeof(short):
            ai = (short **)A->ptr;
            ait = (short **)At->ptr;
            for(i = 0 ; i < A->rows ; i++)
                for(j = 0 ; j < A->cols ; j++)
                    ait[j][i] = ai[i][j];
            break;
        case sizeof(float):
            af = (float **)A->ptr;
            aft = (float **)At->ptr;
            for(i = 0 ; i < A->rows ; i++)
                for(j = 0 ; j < A->cols ; j++)
                    aft[j][i] = af[i][j];
            break;
        case sizeof(double):
            ad = (double **)A->ptr;
            adt = (double **)At->ptr;
            for(i = 0 ; i < A->rows ; i++)
                for(j = 0 ; j < A->cols ; j++)
                    adt[j][i] = ad[i][j];
    }
    return(At);
}

/*
PWISE_MAT MACRO FOR MATRIX add, subtract, mult_pwise functions:

PWISE_MAT(a,b,c,rowsa,colsa,oper,typea,typeb,typec,cast)

    a       pointer to first MATRIX structure.
    b       pointer to second MATRIX structure.
    c       pointer to result MATRIX structure.
    rowsa   number of rows in matrix a
    colsa   number of columns in matrix a
    oper    any legal C operator on the element involved
    typea   legal C type describing the type of a
    typeb   legal C type describing the type of b
    typec   legal C type describing the type of c

*/

#define PWISE_MAT(a,b,c,rowsa,colsa,oper,typea,typeb,typec) {  \
                 typea **_AMX = (typea **)a->ptr;  \
                 typeb **_BMX = (typeb **)b->ptr;  \
                 typec **_CMX = (typec **)c->ptr;  \
                 typea *_PTA;  \
                 typeb *_PTB;  \
                 typec *_PTC;  \
                 int _IX,_JX;  \
                 for(_IX = 0 ; _IX < rowsa ; _IX++) {  \
                     _PTC = _CMX[_IX];  \
                     _PTB = _BMX[_IX];  \
                     _PTA = _AMX[_IX];  \
                     for(_JX = 0 ; _JX < colsa ; _JX++)  \
                         *_PTC++ = (*_PTA++) oper (*_PTB++);  \
                 }  \
             }    

/**************************************************************************

matrix_add - Matrix add, A+B

Uses PWISE_MAT macro to do all 9 cases of matrix types.
Promotes to most precise type of A and B.

    MATRIX *matrix_add(MATRIX *A,MATRIX *B)

*************************************************************************/

MATRIX *matrix_add(A,B)
    MATRIX *A,*B;
{
    MATRIX *matrix_allocate();

    MATRIX *C;
    int a_r,a_c;

    if(B->rows != A->rows || B->cols != A->cols) {
        printf("\nERROR in matrix_add: B and A sizes must agree");
        printf("\nA is %dx%d and B is %dx%d\n"
                             ,A->rows,A->cols,B->rows,B->cols);
        exit(1);
    }

/* simplify the row and col variables */
    a_r = A->rows;
    a_c = A->cols;

/* allocate C to be of the highest ranking type of A and B
   (largest element size gives the highest rank ) */

    if(A->element_size > B->element_size)
         C = matrix_allocate(a_r,a_c,A->element_size);
    else
         C = matrix_allocate(a_r,a_c,B->element_size);

/* do the 9 type cases of A and B with 2 nested switch statements */

    switch(A->element_size) {
        case sizeof(short):
            switch(B->element_size) {
                case sizeof(short):    /* C int, A int, B int */
                    PWISE_MAT(A,B,C,a_r,a_c,+,short,short,short)
                    break;
                case sizeof(float):    /* C float, A int, B float */
                    PWISE_MAT(A,B,C,a_r,a_c,+,short,float,float)
                    break;
                case sizeof(double):    /* C double, A int, B double */
                    PWISE_MAT(A,B,C,a_r,a_c,+,short,double,double)
            }
            break;
        case sizeof(float):
            switch(B->element_size) {
                case sizeof(short):    /* C float, A float, B int */
                    PWISE_MAT(A,B,C,a_r,a_c,+,float,short,float)
                    break;
                case sizeof(float):    /* C float, A float, B float */
                    PWISE_MAT(A,B,C,a_r,a_c,+,float,float,float)
                    break;
                case sizeof(double):    /* C double, A float, B double */
                    PWISE_MAT(A,B,C,a_r,a_c,+,float,double,double)
            }
            break;
        case sizeof(double):
            switch(B->element_size) {
                case sizeof(short):    /* C double, A double, B int */
                    PWISE_MAT(A,B,C,a_r,a_c,+,double,short,double)
                    break;
                case sizeof(float):    /* C double, A double, B float */
                    PWISE_MAT(A,B,C,a_r,a_c,+,double,float,double)
                    break;
                case sizeof(double):    /* C double, A double, B double */
                    PWISE_MAT(A,B,C,a_r,a_c,+,double,double,double)
            }
            break;
    }
    return(C);
}

/**************************************************************************

matrix_sub - Matrix subtract, A-B

Uses PWISE_MAT macro to do all 9 cases of matrix types.
Promotes to most precise type of A and B.

    MATRIX *matrix_sub(MATRIX *A,MATRIX *B)

*************************************************************************/

MATRIX *matrix_sub(A,B)
    MATRIX *A,*B;
{
    MATRIX *matrix_allocate();

    MATRIX *C;
    int a_r,a_c;

    if(B->rows != A->rows || B->cols != A->cols) {
        printf("\nERROR in matrix_sub: B and A sizes must agree");
        printf("\nA is %dx%d and B is %dx%d\n"
                             ,A->rows,A->cols,B->rows,B->cols);
        exit(1);
    }

/* simplify the row and col variables */
    a_r = A->rows;
    a_c = A->cols;

/* allocate C to be of the highest ranking type of A and B
   (largest element size gives the highest rank ) */

    if(A->element_size > B->element_size)
         C = matrix_allocate(a_r,a_c,A->element_size);
    else
         C = matrix_allocate(a_r,a_c,B->element_size);

/* do the 9 type cases of A and B with 2 nested switch statements */

    switch(A->element_size) {
        case sizeof(short):
            switch(B->element_size) {
                case sizeof(short):    /* C int, A int, B int */
                    PWISE_MAT(A,B,C,a_r,a_c,-,short,short,short)
                    break;
                case sizeof(float):    /* C float, A int, B float */
                    PWISE_MAT(A,B,C,a_r,a_c,-,short,float,float)
                    break;
                case sizeof(double):    /* C double, A int, B double */
                    PWISE_MAT(A,B,C,a_r,a_c,-,short,double,double)
            }
            break;
        case sizeof(float):
            switch(B->element_size) {
                case sizeof(short):    /* C float, A float, B int */
                    PWISE_MAT(A,B,C,a_r,a_c,-,float,short,float)
                    break;
                case sizeof(float):    /* C float, A float, B float */
                    PWISE_MAT(A,B,C,a_r,a_c,-,float,float,float)
                    break;
                case sizeof(double):    /* C double, A float, B double */
                    PWISE_MAT(A,B,C,a_r,a_c,-,float,double,double)
            }
            break;
        case sizeof(double):
            switch(B->element_size) {
                case sizeof(short):    /* C double, A double, B int */
                    PWISE_MAT(A,B,C,a_r,a_c,-,double,short,double)
                    break;
                case sizeof(float):    /* C double, A double, B float */
                    PWISE_MAT(A,B,C,a_r,a_c,-,double,float,double)
                    break;
                case sizeof(double):    /* C double, A double, B double */
                    PWISE_MAT(A,B,C,a_r,a_c,-,double,double,double)
            }
            break;
    }       
    return(C);
}

/**************************************************************************

matrix_mult_pwise - Matrix mult point wise on data, A.*B.

Uses PWISE_MAT macro to do all 9 cases of matrix types.
Promotes to most precise type of A and B.

    MATRIX *matrix_mult_pwise(MATRIX *A,MATRIX *B)

*************************************************************************/

MATRIX *matrix_mult_pwise(A,B)
    MATRIX *A,*B;
{
    MATRIX *matrix_allocate();

    MATRIX *C;
    int a_r,a_c;

    if(B->rows != A->rows || B->cols != A->cols) {
        printf("\nERROR in matrix_mult_pwise: B and A sizes must agree");
        printf("\nA is %dx%d and B is %dx%d\n"
                             ,A->rows,A->cols,B->rows,B->cols);
        exit(1);
    }

/* simplify the row and col variables */
    a_r = A->rows;
    a_c = A->cols;

/* allocate C to be of the highest ranking type of A and B
   (largest element size gives the highest rank ) */

    if(A->element_size > B->element_size)
         C = matrix_allocate(a_r,a_c,A->element_size);
    else
         C = matrix_allocate(a_r,a_c,B->element_size);

/* do the 9 type cases of A and B with 2 nested switch statements */

    switch(A->element_size) {
        case sizeof(short):
            switch(B->element_size) {
                case sizeof(short):    /* C int, A int, B int */
                    PWISE_MAT(A,B,C,a_r,a_c,*,short,short,short)
                    break;
                case sizeof(float):    /* C float, A int, B float */
                    PWISE_MAT(A,B,C,a_r,a_c,*,short,float,float)
                    break;
                case sizeof(double):    /* C double, A int, B double */
                    PWISE_MAT(A,B,C,a_r,a_c,*,short,double,double)
            }
            break;
        case sizeof(float):
            switch(B->element_size) {
                case sizeof(short):    /* C float, A float, B int */
                    PWISE_MAT(A,B,C,a_r,a_c,*,float,short,float)
                    break;
                case sizeof(float):    /* C float, A float, B float */
                    PWISE_MAT(A,B,C,a_r,a_c,*,float,float,float)
                    break;
                case sizeof(double):    /* C double, A float, B double */
                    PWISE_MAT(A,B,C,a_r,a_c,*,float,double,double)
            }
            break;
        case sizeof(double):
            switch(B->element_size) {
                case sizeof(short):    /* C double, A double, B int */
                    PWISE_MAT(A,B,C,a_r,a_c,*,double,short,double)
                    break;
                case sizeof(float):    /* C double, A double, B float */
                    PWISE_MAT(A,B,C,a_r,a_c,*,double,float,double)
                    break;
                case sizeof(double):    /* C double, A double, B double */
                    PWISE_MAT(A,B,C,a_r,a_c,*,double,double,double)
            }
    }       
    return(C);
}

/**************************************************************************

matrix_mult - Matrix cross product, A*B

Gives error and calls exit if size of A and B are not correct.
Uses MULT_MAT macro to do all 9 cases of matrix types.
Promotes to most precise type of A and B.

    MATRIX *matrix_mult(MATRIX *A,MATRIX *B)

*************************************************************************/

/*
MULT_MAT MACRO FOR MATRIX MULTIPLICATION:

MULT_MAT(a,b,c,rowsa,colsa,colsb,typea,typeb,typec)

    a       pointer to first MATRIX structure.
    b       pointer to second MATRIX structure.
    c       pointer to result MATRIX structure.
    rowsa   number of rows in matrix a
    colsa   number of columns in matrix a
    colsb   number of columns in matrix b
    typea   legal C type describing the type of a
    typeb   legal C type describing the type of b
    typec   legal C type describing the type of c

*/

#define MULT_MAT(a,b,c,rowsa,colsa,colsb,typea,typeb,typec) {  \
                 typea **_AMX = (typea **)a->ptr;  \
                 typeb **_BMX = (typeb **)b->ptr;  \
                 typec **_CMX = (typec **)c->ptr;  \
                 typea *_PTA;  \
                 typeb *_PTB;  \
                 typec *_PTC;  \
                 int _IX,_JX,_KX;  \
                 for(_IX = 0 ; _IX < rowsa ; _IX++) {  \
                     _PTC = _CMX[_IX];  \
                     _PTB = _BMX[0];  \
                     for(_JX = 0 ; _JX < colsb ; _JX++) {  \
                         _PTA = _AMX[_IX];  \
                         *_PTC = (*_PTA++) * (*_PTB++);  \
                         for(_KX = 1 ; _KX < colsa ; _KX++)  \
                             *_PTC += (*_PTA++)* _BMX[_KX][_JX];  \
                         _PTC++;  \
                     }  \
                 }  \
             }    

MATRIX *matrix_mult(A,B)
    MATRIX *A,*B;
{
    MATRIX *matrix_allocate();

    MATRIX *C;
    int a_r,a_c,b_c;

    if(B->rows != A->cols) {
        printf("\nERROR in matrix_mult: B row and A col sizes must agree");
        printf("\nA is %dx%d and B is %dx%d\n"
                             ,A->rows,A->cols,B->rows,B->cols);
        exit(1);
    }

    if(!A->ptr) {
        printf("\nERROR: No A matrix allocated for matrix_mult");
        exit(1);
    }
    if(!B->ptr) {
        printf("\nERROR: No B matrix allocated for matrix_mult");
        exit(1);
    }

/* simplify the row and col variables */
    a_r = A->rows;
    a_c = A->cols;
    b_c = B->cols;

/* allocate C to be of the highest ranking type of A and B
   (largest element size gives the highest rank ) */

    if(A->element_size > B->element_size)
         C = matrix_allocate(a_r,b_c,A->element_size);
    else
         C = matrix_allocate(a_r,b_c,B->element_size);

/* do the 9 type cases of A and B with 2 nested switch statements */

    switch(A->element_size) {
        case sizeof(short):
            switch(B->element_size) {
                case sizeof(short):    /* C int, A int, B int */
                    MULT_MAT(A,B,C,a_r,a_c,b_c,short,short,short)
                    break;
                case sizeof(float):    /* C float, A int, B float */
                    MULT_MAT(A,B,C,a_r,a_c,b_c,short,float,float)
                    break;
                case sizeof(double):    /* C double, A int, B double */
                    MULT_MAT(A,B,C,a_r,a_c,b_c,short,double,double)
            }
            break;
        case sizeof(float):
            switch(B->element_size) {
                case sizeof(short):    /* C float, A float, B int */
                    MULT_MAT(A,B,C,a_r,a_c,b_c,float,short,float)
                    break;
                case sizeof(float):    /* C float, A float, B float */
                    MULT_MAT(A,B,C,a_r,a_c,b_c,float,float,float)
                    break;
                case sizeof(double):    /* C double, A float, B double */
                    MULT_MAT(A,B,C,a_r,a_c,b_c,float,double,double)
            }
            break;
        case sizeof(double):
            switch(B->element_size) {
                case sizeof(short):    /* C double, A double, B int */
                    MULT_MAT(A,B,C,a_r,a_c,b_c,double,short,double)
                    break;
                case sizeof(float):    /* C double, A double, B float */
                    MULT_MAT(A,B,C,a_r,a_c,b_c,double,float,double)
                    break;
                case sizeof(double):    /* C double, A double, B double */
                    MULT_MAT(A,B,C,a_r,a_c,b_c,double,double,double)
            }
    }       
    return(C);
}

/**************************************************************************

matrix_invert - Gauss-Jordan elimination with full pivoting.

The returned matrix is always a double matrix.  Exits and prints
error message with singular matrices or bad MATRIX structures.

    MATRIX *matrix_invert(MATRIX *A)

*************************************************************************/

MATRIX *matrix_invert(A)
    MATRIX *A;
{
    MATRIX *matrix_allocate();

    MATRIX *Ai;
    double **a;
    double big,pivot_inverse,temp,abs_element;
    int *pivot_flag,*swap_col,*swap_row;
    int i,n,row,col,swap,irow,icol;

/* check for square matrix */
    if(A->rows != A->cols) {
        printf("\nmatrix_invert ERROR: non-square, size = %d x %d",
            A->rows,A->cols);
        exit(1);
    }

/* check pointer */
    if(!A->ptr) {
        printf("\nERROR: No matrix for matrix_invert");
        exit(1);
    }

/* size of square matrix */
    n = A->rows;

/* allocate space for the inverse */
    Ai = matrix_allocate(n,n,sizeof(double));

/* copy to double matrix */
    switch(A->element_size) {
        case sizeof(short):
            SCALE_MAT(A,Ai,1,n,n,0,0,short,double)
            break;
        case sizeof(float):
            SCALE_MAT(A,Ai,1,n,n,0,0,float,double)
            break;
        case sizeof(double):
            SCALE_MAT(A,Ai,1,n,n,0,0,double,double)
            break;
        default:
            printf("\nERROR in matrix_invert: unsupported type\n");
            exit(1);
    }

    a = (double **)Ai->ptr;

/* allocate index arrays and set to zero */
    pivot_flag = (int *) calloc(n,sizeof(int));
    swap_row = (int *) calloc(n,sizeof(int));
    swap_col = (int *) calloc(n,sizeof(int));

    if(!pivot_flag || !swap_row || !swap_col) {
        printf("\nAllocation error in matrix_invert\n");
        exit(1);
    }

    for(i = 0 ; i < n ; i++) {   /* n iterations of pivoting */

/* find the biggest pivot element */
        big = 0.0;
        for(row = 0 ; row < n ; row++) {
            if(!pivot_flag[row]) {       /* only unused pivots */
                for(col = 0 ; col < n ; col++) {
                    if(!pivot_flag[col]) {
                        abs_element = fabs(a[row][col]);
                        if(abs_element >= big) {
                            big = abs_element;
                            irow = row;
                            icol = col;
                        }
                    }
                }
            }
        }
        pivot_flag[icol]++;    /* mark this pivot as used */

/* swap rows to make this diagonal the biggest absolute pivot */
        if(irow != icol) {
            for(col = 0 ; col < n ; col++) {
                temp = a[irow][col];
                a[irow][col] = a[icol][col];
                a[icol][col] = temp;
            }
       }

/* store what we swaped */
       swap_row[i] = irow;
       swap_col[i] = icol;

/* bad news if the pivot is zero */
       if(a[icol][icol] == 0.0) {
           printf("\nmatrix_invert ERROR: SINGULAR MATRIX\n");
           exit(1);
       }

/* divide the row by the pivot */
       pivot_inverse = 1.0/a[icol][icol];
       a[icol][icol] = 1.0;        /* pivot = 1 to avoid round off */
       for(col = 0 ; col < n ; col++)
           a[icol][col] = a[icol][col]*pivot_inverse;

/* fix the other rows by subtracting */
       for(row = 0 ; row < n ; row++) {
           if(row != icol) {
               temp = a[row][icol];
               a[row][icol] = 0.0;
               for(col = 0 ; col < n ; col++)
                   a[row][col] = a[row][col]-a[icol][col]*temp;
           }
        }
    }

/* fix the affect of all the swaps for final answer */
    for(swap = n-1 ; swap >= 0 ; swap--) {
        if(swap_row[swap] != swap_col[swap]) {
            for(row = 0 ; row < n ; row++) {
                temp = a[row][swap_row[swap]];
                a[row][swap_row[swap]] = a[row][swap_col[swap]];
                a[row][swap_col[swap]] = temp;
            }
        }
    }

/* free up all the index arrays */
    free((char *)pivot_flag);
    free((char *)swap_row);
    free((char *)swap_col);

    return(Ai);
}

/**************************************************************************

matrix_det - Determinant of the input matrix.

Returns a double value equal to the determinant of the matrix.
Allocates and frees a matrix for the determinant calculation.
Exits and prints error message on invalid MATRIX structure.

    double matrix_det(MATRIX *A)

*************************************************************************/

double matrix_det(A)
    MATRIX *A;
{
    MATRIX *matrix_allocate();
    void matrix_free();

    MATRIX *Adet;
    double **a;
    double *a_ptr;
    double det,big,pivot_inverse,temp,abs_element;
    int n,row,col,swap_row,pivot;

    if(A->rows != A->cols) {
        printf("\nmatrix_det ERROR: non-square, size = %d x %d",
            A->rows,A->cols);
        exit(1);
    }

/* check pointer */
    if(!A->ptr) {
        printf("\nERROR: No matrix for matrix_det\n");
        exit(1);
    }

/* size of square matrix */
    n = A->rows;

/* allocate space for the determinant calculation matrix */
    Adet = matrix_allocate(n,n,sizeof(double));

/* copy to double matrix for calculations */
    switch(A->element_size) {
        case sizeof(short):
            SCALE_MAT(A,Adet,1,n,n,0,0,short,double)
            break;
        case sizeof(float):
            SCALE_MAT(A,Adet,1,n,n,0,0,float,double)
            break;
        case sizeof(double):
            SCALE_MAT(A,Adet,1,n,n,0,0,double,double)
            break;
        default:
            printf("\nERROR in matrix_invert: unsupported type\n");
            exit(1);
    }

    a = (double **)Adet->ptr;

/* initialize the answer */
    det = 1.0;

    for(pivot = 0 ; pivot < n-1 ; pivot++) {

/* find the biggest absolute pivot */
        big = fabs(a[pivot][pivot]);
        swap_row = 0;               /* initialize for no swap */
        for(row = pivot + 1; row < n ; row++) {
            abs_element = fabs(a[row][pivot]);
            if(abs_element > big) {
                swap_row = row;
                big = abs_element;
            }
        }

/* unless swap_row is still zero we must swap two rows */
        if(swap_row != 0) {
            a_ptr = a[pivot];
            a[pivot] = a[swap_row];
            a[swap_row] = a_ptr;
/* change the sign of determinant because of swap */
            det = -det*a[pivot][pivot];
        }
        else {
/* calculate the determinant by the product of the pivots */
            det = det*a[pivot][pivot];
        }

/* if almost singular matrix, give up now */
        if(fabs(det) < 1.0e-50) return(det);

        pivot_inverse = 1.0/a[pivot][pivot];
        for(col = pivot + 1 ; col < n ; col++) {
            a[pivot][col] = a[pivot][col]*pivot_inverse;
        }

        for(row = pivot + 1 ; row < n ; row++) {
            temp = a[row][pivot];
            for(col = pivot + 1 ; col < n ; col++) {
                a[row][col] = a[row][col] - a[pivot][col]*temp;
            }
        }
    }

/* last pivot, no reduction required */
    det = det*a[n-1][n-1];

/* free up the calculation matrix */
    matrix_free(Adet);

    return(det);
}
