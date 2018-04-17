#include <stdlib.h>
#include <stdio.h>
#include "vector.h"

/**************************************************************************

VECTST.C - TEST VECTOR MACROS

INPUTS: NONE

OUTPUTS: PRINT OF TEST VECTORS AND RESULTS

TESTS THE FOLLOWING MACROS:

ADD_VEC(a,b,c,len,typea,typeb,typec)       add vectors, c = a + b
SUB_VEC(a,b,c,len,typea,typeb,typec)       subtract vectors, c = a - b
MULT_VEC(a,b,c,len,typea,typeb,typec)      multiply vectors, c = a * b
DOTP_VEC(a,b,s,len,typea,typeb)            dot product, c=sum(a*b)
SUM_VEC(a,s,len,typea)                     sum of vector, s=sum(a)
SCALE_VEC(a,b,s,len,typea,typeb)           scale and copy a vector
                                           by a constant, b = x * a

*************************************************************************/

main()
{
    static float x[5] = { 10.0, 9.0, 8.0, 7.0, 6.0 };
    static float y[8] = { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0};
    float z[5];
    int iz[5],iy[5];
    float sum;
    int i;

/* sum of two floats */
    ADD_VEC(x,y,z,5,float,float,float)
    printf("\nVector sum: z=x+y\n     x      y      z");
    for(i = 0 ; i < 5 ; i++)
        printf("\n %6.1f %6.1f %6.1f",x[i],y[i],z[i]);

/* difference of two floats */
    SUB_VEC(x,y,z,5,float,float,float)
    printf("\nVector subtract: z=x-y\n     x      y      z");
    for(i = 0 ; i < 5 ; i++)
        printf("\n %6.1f %6.1f %6.1f",x[i],y[i],z[i]);

/* multiply two floats */
    MULT_VEC(x,y,z,5,float,float,float)
    printf("\nVector product: z=x*y\n     x      y      z");
    for(i = 0 ; i < 5 ; i++)
        printf("\n %6.1f %6.1f %6.1f",x[i],y[i],z[i]);

/* offset multiply of two floats */
    MULT_VEC(x,y+2,z,5,float,float,float)
    printf("\nVector product: z=x[i]*y[i+2])\n     x    y[i+2]   z");
    for(i = 0 ; i < 5 ; i++)
        printf("\n %6.1f %6.1f %6.1f",x[i],y[i+2],z[i]);

/* dot product x.*y */
    DOTP_VEC(x,y,sum,5,float,float)
    printf("\n\nDot product of x and y = %f",sum);

/* sum of x */
    SUM_VEC(x,sum,5,float)
    printf("\nSum of x = %f",sum);

/* scale of float to another float */
    SCALE_VEC(x,z,10,5,float,float)
    printf("\n\nScale vector: z=10*x\n     x      y      z");
    for(i = 0 ; i < 5 ; i++)
        printf("\n %6.1f %6.1f %6.1f",x[i],y[i],z[i]);

/* scale of float into integer */
    SCALE_VEC(x,iz,13,5,float,int)
    printf("\nScale into integer vector: iz=13*x\n     x      y     iz");
    for(i = 0 ; i < 5 ; i++)
        printf("\n %6.1f %6.1f %6d",x[i],y[i],iz[i]);

/* self scale of float */
    SCALE_VEC(x,x,130,5,float,float)
    printf("\nSelf Scale into x vector: x=130*x\n     x      y     z");
    for(i = 0 ; i < 5 ; i++)
        printf("\n %6.1f %6.1f %6.1f",x[i],y[i],z[i]);

/* multiply into int vector */
    MULT_VEC(x,y,iz,5,float,float,int)
    printf("\nVector product to integer: iz=x*y\n     x      y      iz");
    for(i = 0 ; i < 5 ; i++)
        printf("\n %6.1f %6.1f %6d",x[i],y[i],iz[i]);

/* scale by float into int */
    SCALE_VEC(y,iy,23.7,5,float,int)
    printf("\nScale into integer vector: iy=23.7*y\n     x      y     iy");
    for(i = 0 ; i < 5 ; i++)
        printf("\n %6.1f %6.1f %6d",x[i],y[i],iy[i]);

/* product of int and float into float */
    MULT_VEC(y,iy,z,5,float,int,float)
    printf("\nVector product into float: z=y*iy\n     y     iy      z");
    for(i = 0 ; i < 5 ; i++)
        printf("\n %6.1f %6d %6.1f",y[i],iy[i],z[i]);
}
