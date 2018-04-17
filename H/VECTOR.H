/**************************************************************************

VECTOR.H - GENERIC VECTOR MACROS INCLUDE FILE

Performs operations on vectors (arrays) of data.  Must give
pointers and type of data string to each macro.  A string
indicating the type of each vector is used to allocate a temp
pointer for use within each macro.  This allows the macros to
work with any legal c data type (int, float, double, unsigned
int, char, unsigned char, ect.)  More than one pointer can point
to the same vector, if desired.

More than one pointer can point to the same vector, if desired.
Thus, a = a*b can be obtained with MULT_VEC(a,b,a,10,int,int,int)
where a and b are 10 element int arrays.

Pointer arguments (a,b,c) can be expressions since they are used
only once.  Thus, ADD_VEC(z+2,x+5,y,10,int,int,int)
will work as expected on the 10 elements begining at z+2 and x+5
(note that x, y, and z are all int arrays of data).

CONTAINS THE FOLLOWING MACROS:

ADD_VEC(a,b,c,len,typea,typeb,typec)       add vectors, c = a + b
SUB_VEC(a,b,c,len,typea,typeb,typec)       subtract vectors, c = a - b
MULT_VEC(a,b,c,len,typea,typeb,typec)      multiply vectors, c = a * b
DOTP_VEC(a,b,s,len,typea,typeb)            dot product s=sum(a*b)
SUM_VEC(a,s,len,typea)                     sum of vector, s=sum(a)
SCALE_VEC(a,b,s,len,typea,typeb)           scale and copy a vector
                                           by a constant, b = x * a

*************************************************************************/

/*
ADD_VEC macro:

ADDS TWO VECTORS (a,b) POINT BY POINT (PROMOTING AS REQUIRED) AND 
PUTS THE RESULT IN THE c VECTOR (DEMOTING IF REQUIRED).

ADD_VEC(a,b,c,len,typea,typeb,typec)

    a       pointer to first vector.
    b       pointer to second vector.
    c       pointer to result vector.
    len     length of vectors (integer).
    typea   legal C type describing the type of a data.
    typeb   legal C type describing the type of b data.
    typec   legal C type describing the type of c data.
*/

#define ADD_VEC(a,b,c,len,typea,typeb,typec) {  \
                  typea *_PTA = a;  \
                  typeb *_PTB = b;  \
                  typec *_PTC = c;  \
                  int _IX;  \
                      for(_IX = 0 ; _IX < (len) ; _IX++)  \
                          *_PTC++ = (typec)((*_PTA++) + (*_PTB++));  \
                  }

/*
SUB_VEC macro:

SUBTRACTS TWO VECTORS (a,b) POINT BY POINT (PROMOTING AS REQUIRED) AND
PUTS THE RESULT IN THE c VECTOR (DEMOTING IF REQUIRED).

SUB_VEC(a,b,c,len,typea,typeb,typec)

    a       pointer to first vector.
    b       pointer to second vector.
    c       pointer to result vector.
    len     length of vectors (integer).
    typea   legal C type describing the type of a data.
    typeb   legal C type describing the type of b data.
    typec   legal C type describing the type of c data.

*/

#define SUB_VEC(a,b,c,len,typea,typeb,typec) {  \
                  typea *_PTA = a;  \
                  typeb *_PTB = b;  \
                  typec *_PTC = c;  \
                  int _IX;  \
                      for(_IX = 0 ; _IX < (len) ; _IX++)  \
                          *_PTC++ = (typec)((*_PTA++) - (*_PTB++));  \
                  }

/*
MULT_VEC macro:

MULTIPLIES TWO VECTORS (a,b) POINT BY POINT (PROMOTING AS REQUIRED) AND
PUTS THE RESULT IN THE c VECTOR (DEMOTING IF REQUIRED).

MULT_VEC(a,b,c,len,typea,typeb,typec)

    a       pointer to first vector.
    b       pointer to second vector.
    c       pointer to result vector.
    len     length of vectors (integer).
    typea   legal C type describing the type of a data.
    typeb   legal C type describing the type of b data.
    typec   legal C type describing the type of c data.

WARNING: The input data vectors are not cast to the type of c.
         This means that at least one of the input types must
         be able to represent the individual products without
         overflow.

*/

#define MULT_VEC(a,b,c,len,typea,typeb,typec) {  \
                   typea *_PTA = a;  \
                   typeb *_PTB = b;  \
                   typec *_PTC = c;  \
                   int _IX;  \
                       for(_IX = 0 ; _IX < (len) ; _IX++)  \
                           *_PTC++ = (typec)((*_PTA++) * (*_PTB++));  \
                   }

/*
DOTP_VEC macro:

FORMS THE SUM OF PRODUCTS OF TWO VECTORS (a,b) AND
PUTS THE RESULT IN THE PREVIOUSLY DEFINED VARIABLE s.

DOTP_VEC(a,b,s,len,typea,typeb)

    a       pointer to first vector.
    b       pointer to second vector.
    s       variable used to store result (not a pointer).
    len     length of vectors (integer).
    typea   legal C type describing the type of a data.
    typeb   legal C type describing the type of b data.

WARNING: The input data vectors are not cast to the type of s.
         This means that at least one of the input types must
         be able to represent the individual products without
         overflow.

*/

#define DOTP_VEC(a,b,s,len,typea,typeb) {  \
                       typea *_PTA = a;  \
                       typeb *_PTB = b;  \
                       int _IX;  \
                       s = (*_PTA++) * (*_PTB++);  \
                       for(_IX = 1 ; _IX < (len) ; _IX++)  \
                           s += (*_PTA++) * (*_PTB++);  \
                   }
/*
SUM_VEC macro:

FORMS THE SUM THE VECTOR a AND PUT THE RESULT IN THE
PREVIOUSLY DEFINED VARIABLE s.

SUM_VEC(a,s,len,typea)

    a       pointer to first vector.
    s       variable used to store result (not a pointer).
    len     length of vector (integer).
    typea   legal C type describing the type of a data.

*/

#define SUM_VEC(a,s,len,typea) {  \
                       typea *_PTA = a;  \
                       int _IX;  \
                       s = (*_PTA++);  \
                       for(_IX = 1 ; _IX < (len) ; _IX++)  \
                           s += (*_PTA++);  \
                   }

/*
SCALE_VEC macro:

SCALES AND/OR CONVERTS (PROMOTES OR DEMOTES) THE INPUT VECTOR a
(of typea) AND COPIES THE SCALED VECTOR INTO ANOTHER VECTOR b
(of typeb).

SCALE_VEC(a,b,s,len,typea,typeb)

    a       pointer to input vector.
    b       pointer to output vector.
    s       variable used to scale output vector (not a pointer).
    len     length of vectors (integer).
    typea   legal C type describing the type of a data.
    typeb   legal C type describing the type of b data.

*/

#define SCALE_VEC(a,b,s,len,typea,typeb) {  \
                       typea *_PTA = (typea *)a;  \
                       typeb *_PTB = (typeb *)b;  \
                       int _IX;  \
                       for(_IX = 0 ; _IX < (len) ; _IX++)  \
                           *(_PTB)++ = (typeb)(s * (*(_PTA)++));  \
                    }
