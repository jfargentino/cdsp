#include    <math.h>
#include    <stdlib.h>
#include    <stdio.h>
#include    "dft.h"

/**********************************************************************

DFT.C - SOURCE CODE FOR DISCRETE FOURIER TRANSFORM FUNCTIONS

dft     Discrete Fourier Transform
idft    Inverse Discrete Fourier Transform
fft     In-place radix 2 decimation in time FFT
ifft    In-place radix 2 decimation in time inverse FFT
rfft    Trig recombination real input FFT
ham     Hamming window
han     Hanning window
triang  Triangle window
black   Blackman window
harris  4 term Blackman-Harris window
log2    Base 2 logarithm

***********************************************************************/

/***********************************************************************

dft - Discrete Fourier Transform
  
This function performs a straight DFT of N points on an array of
complex numbers whose first member is pointed to by Datain.  The
output is placed in an array pointed to by Dataout.

void dft(COMPLEX *Datain, COMPLEX *Dataout, int N)

*************************************************************************/

void dft(Datain, Dataout, N)
    COMPLEX *Datain, *Dataout;
    int N;
{
    int i,k,n,p;
    static int nstore = 0;      /* store N for future use */
    static COMPLEX *cf;         /* coefficient storage */
    COMPLEX *cfptr,*Dinptr;
    double arg;

/* Create the coefficients if N has changed */

    if(N != nstore) {
        if(nstore != 0) free((char *) cf);    /* free previous */

        cf = (COMPLEX  *) calloc(N, sizeof(COMPLEX));
        if (!cf) {
            printf("\nUnable to allocate memory for coefficients.\n");
            exit(1);
        }

        arg = 8.0*atan(1.0)/N;
        for (i=0 ; i<N ; i++) {
            cf[i].real = (float)cos(arg*i);
            cf[i].imag = -(float)sin(arg*i);
        }
    }

/* Perform the DFT calculation */

    printf("\n");
    for (k=0 ; k<N ; k++) {

        Dinptr = Datain;
        Dataout->real = Dinptr->real;
        Dataout->imag = Dinptr->imag;
        Dinptr++;
        for (n=1; n<N; n++) {

        p = (int)((long)n*k % N);
            cfptr = cf + p;         /* pointer to cf modulo N */

            Dataout->real += Dinptr->real * cfptr->real
                             - Dinptr->imag * cfptr->imag;

            Dataout->imag += Dinptr->real * cfptr->imag
                             + Dinptr->imag * cfptr->real;
            Dinptr++;
        }
        if (k % 32 == 31) printf("*");
        Dataout++;          /* next output */
    }
    printf("\n");
}

/***********************************************************************

idft - Inverse Discrete Fourier Transform
  
This function performs an inverse DFT of N points on an array of
complex numbers whose first member is pointed to by Datain.  The
output is placed in an array pointed to by Dataout.
It returns nothing.

void idft(COMPLEX *Datain, COMPLEX *Dataout, int N)

*************************************************************************/

void idft(Datain, Dataout, N)
    COMPLEX *Datain, *Dataout;
    int N;
{
    int i,k,n,p;
    static int nstore = 0;      /* store N for future use */
    static COMPLEX *cf;         /* coefficient storage */
    COMPLEX *cfptr,*Dinptr;
    double arg;

/* Create the coefficients if N has changed */

    if(N != nstore) {
        if(nstore != 0) free((char *) cf);    /* free previous */

        cf = (COMPLEX  *) calloc(N, sizeof(COMPLEX));
        if (cf == 0) {
            printf("\nUnable to allocate memory for coefficients.\n");
            exit(1);
        }

/* scale stored values by 1/N */
        arg = 8.0*atan(1.0)/N;
        for (i=0 ; i<N ; i++) {
            cf[i].real = (float)(cos(arg*i)/(double)N);
            cf[i].imag = (float)(sin(arg*i)/(double)N);
        }
    }

/* Perform the DFT calculation */

    printf("\n");
    for (k=0 ; k<N ; k++) {

        Dinptr = Datain;
        Dataout->real = Dinptr->real * cf[0].real;
        Dataout->imag = Dinptr->imag * cf[0].real;
        Dinptr++;
        for (n=1; n<N; n++) {

        p = (int)((long)n*k % N);
            cfptr = cf + p;         /* pointer to cf modulo N */

            Dataout->real += Dinptr->real * cfptr->real
                             - Dinptr->imag * cfptr->imag;

            Dataout->imag += Dinptr->real * cfptr->imag
                             + Dinptr->imag * cfptr->real;
            Dinptr++;
        }
        if (k % 32 == 31) printf("*");
        Dataout++;          /* next output */
    }
    printf("\n");
}

/**************************************************************************

fft - In-place radix 2 decimation in time FFT

Requires pointer to complex array, x and power of 2 size of FFT, m
(size of FFT = 2**m).  Places FFT output on top of input COMPLEX array.

void fft(COMPLEX *x, int m)

*************************************************************************/

void fft(x,m)
    COMPLEX *x;
    int m;
{
    static COMPLEX *w;           /* used to store the w complex array */
    static int mstore = 0;       /* stores m for future reference */
    static int n = 1;            /* length of fft stored for future */

    COMPLEX u,temp,tm;
    COMPLEX *xi,*xip,*xj,*wptr;

    int i,j,k,l,le,windex;

    double arg,w_real,w_imag,wrecur_real,wrecur_imag,wtemp_real;

    if(m != mstore) {

/* free previously allocated storage and set new m */

        if(mstore != 0) free(w);
        mstore = m;
        if(m == 0) return;       /* if m=0 then done */

/* n = 2**m = fft length */

        n = 1 << m;
        le = n/2;

/* allocate the storage for w */

        w = (COMPLEX *) calloc(le-1,sizeof(COMPLEX));
        if(!w) {
            printf("\nUnable to allocate complex W array\n");
            exit(1);
        }

/* calculate the w values recursively */

        arg = 4.0*atan(1.0)/le;         /* PI/le calculation */
        wrecur_real = w_real = cos(arg);
        wrecur_imag = w_imag = -sin(arg);
        xj = w;
        for (j = 1 ; j < le ; j++) {
            xj->real = (float)wrecur_real;
            xj->imag = (float)wrecur_imag;
            xj++;
            wtemp_real = wrecur_real*w_real - wrecur_imag*w_imag;
            wrecur_imag = wrecur_real*w_imag + wrecur_imag*w_real;
            wrecur_real = wtemp_real;
        }
    }

/* start fft */

    le = n;
    windex = 1;
    for (l = 0 ; l < m ; l++) {
        le = le/2;

/* first iteration with no multiplies */

        for(i = 0 ; i < n ; i = i + 2*le) {
            xi = x + i;
            xip = xi + le;
            temp.real = xi->real + xip->real;
            temp.imag = xi->imag + xip->imag;
            xip->real = xi->real - xip->real;
            xip->imag = xi->imag - xip->imag;
            *xi = temp;
        }

/* remaining iterations use stored w */

        wptr = w + windex - 1;
        for (j = 1 ; j < le ; j++) {
            u = *wptr;
            for (i = j ; i < n ; i = i + 2*le) {
                xi = x + i;
                xip = xi + le;
                temp.real = xi->real + xip->real;
                temp.imag = xi->imag + xip->imag;
                tm.real = xi->real - xip->real;
                tm.imag = xi->imag - xip->imag;             
                xip->real = tm.real*u.real - tm.imag*u.imag;
                xip->imag = tm.real*u.imag + tm.imag*u.real;
                *xi = temp;
            }
            wptr = wptr + windex;
        }
        windex = 2*windex;
    }            

/* rearrange data by bit reversing */

    j = 0;
    for (i = 1 ; i < (n-1) ; i++) {
        k = n/2;
        while(k <= j) {
            j = j - k;
            k = k/2;
        }
        j = j + k;
        if (i < j) {
            xi = x + i;
            xj = x + j;
            temp = *xj;
            *xj = *xi;
            *xi = temp;
        }
    }
}

/**************************************************************************

ifft - In-place radix 2 decimation in time inverse FFT

Requires pointer to complex array, x and power of 2 size of FFT, m
(size of FFT = 2**m).  Places inverse FFT output on top of input
frequency domain COMPLEX array.

void ifft(COMPLEX *x, int m)

*************************************************************************/

void ifft(x,m)
    COMPLEX *x;
    int m;
{
    static COMPLEX *w;           /* used to store the w complex array */
    static int mstore = 0;       /* stores m for future reference */
    static int n = 1;            /* length of ifft stored for future */

    COMPLEX u,temp,tm;
    COMPLEX *xi,*xip,*xj,*wptr;

    int i,j,k,l,le,windex;

    double arg,w_real,w_imag,wrecur_real,wrecur_imag,wtemp_real;
    float scale;

    if(m != mstore) {

/* free previously allocated storage and set new m */

        if(mstore != 0) free(w);
        mstore = m;
        if(m == 0) return;       /* if m=0 then done */

/* n = 2**m = inverse fft length */

        n = 1 << m;
        le = n/2;

/* allocate the storage for w */

        w = (COMPLEX *) calloc(le-1,sizeof(COMPLEX));
        if(!w) {
            printf("\nUnable to allocate complex W array\n");
            exit(1);
        }

/* calculate the w values recursively */

        arg = 4.0*atan(1.0)/le;         /* PI/le calculation */
        wrecur_real = w_real = cos(arg);
        wrecur_imag = w_imag = sin(arg);  /* opposite sign from fft */
        xj = w;
        for (j = 1 ; j < le ; j++) {
            xj->real = (float)wrecur_real;
            xj->imag = (float)wrecur_imag;
            xj++;
            wtemp_real = wrecur_real*w_real - wrecur_imag*w_imag;
            wrecur_imag = wrecur_real*w_imag + wrecur_imag*w_real;
            wrecur_real = wtemp_real;
        }
    }

/* start inverse fft */

    le = n;
    windex = 1;
    for (l = 0 ; l < m ; l++) {
        le = le/2;

/* first iteration with no multiplies */

        for(i = 0 ; i < n ; i = i + 2*le) {
            xi = x + i;
            xip = xi + le;
            temp.real = xi->real + xip->real;
            temp.imag = xi->imag + xip->imag;
            xip->real = xi->real - xip->real;
            xip->imag = xi->imag - xip->imag;
            *xi = temp;
        }

/* remaining iterations use stored w */

        wptr = w + windex - 1;
        for (j = 1 ; j < le ; j++) {
            u = *wptr;
            for (i = j ; i < n ; i = i + 2*le) {
                xi = x + i;
                xip = xi + le;
                temp.real = xi->real + xip->real;
                temp.imag = xi->imag + xip->imag;
                tm.real = xi->real - xip->real;
                tm.imag = xi->imag - xip->imag;             
                xip->real = tm.real*u.real - tm.imag*u.imag;
                xip->imag = tm.real*u.imag + tm.imag*u.real;
                *xi = temp;
            }
            wptr = wptr + windex;
        }
        windex = 2*windex;
    }            

/* rearrange data by bit reversing */

    j = 0;
    for (i = 1 ; i < (n-1) ; i++) {
        k = n/2;
        while(k <= j) {
            j = j - k;
            k = k/2;
        }
        j = j + k;
        if (i < j) {
            xi = x + i;
            xj = x + j;
            temp = *xj;
            *xj = *xi;
            *xi = temp;
        }
    }

/* scale all results by 1/n */
    scale = (float)(1.0/n);
    for(i = 0 ; i < n ; i++) {
        x->real = scale*x->real;
        x->imag = scale*x->imag;
        x++;
    }
}

/************************************************************

rfft - trig recombination real input FFT

Requires real array pointed to by x, pointer to complex
output array, y and the size of real FFT in power of
2 notation, m (size of input array and FFT, N = 2**m).
On completion, the COMPLEX array pointed to by y 
contains the lower N/2 + 1 elements of the spectrum.

void rfft(float *x, COMPLEX *y, int m)

***************************************************************/

void rfft(x,y,m)
    float      *x;
    COMPLEX    *y;
    int        m;
{
    static    COMPLEX  *cf;
    static    int      mstore = 0;
    int       p,num,k,index;
    float     Realsum, Realdif, Imagsum, Imagdif;
    double    factor, arg;
    COMPLEX   *ck, *xk, *xnk, *cx;

/* First call the fft routine using the x array but with
   half the size of the real fft */

    p = m - 1;
    cx = (COMPLEX *) x;
    fft(cx,p);

/* Next create the coefficients for recombination, if required */

    num = 1 << p;    /* num is half the real sequence length.  */

    if (m!=mstore){
      if (mstore != 0) free(cf);
      cf = (COMPLEX *) calloc(num - 1,sizeof(COMPLEX));
      if(!cf){
        printf("\nUnable to allocate trig recomb coefficients.");
        exit(1);
      }

      factor = 4.0*atan(1.0)/num;
      for (k = 1; k < num; k++){
        arg = factor*k;
        cf[k-1].real = (float)cos(arg);
        cf[k-1].imag = (float)sin(arg);
      }
    }  

/* DC component, no multiplies */
    y[0].real = cx[0].real + cx[0].imag;
    y[0].imag = 0.0;

/* other frequencies by trig recombination */
    ck = cf;
    xk = cx + 1;
    xnk = cx + num - 1;
    for (k = 1; k < num; k++){
      Realsum = ( xk->real + xnk->real ) / 2;
      Imagsum = ( xk->imag + xnk->imag ) / 2;
      Realdif = ( xk->real - xnk->real ) / 2;
      Imagdif = ( xk->imag - xnk->imag ) / 2;

      y[k].real = Realsum + ck->real * Imagsum
                          - ck->imag * Realdif ;

      y[k].imag = Imagdif - ck->imag * Imagsum
                          - ck->real * Realdif ;
      ck++;
      xk++;
      xnk--;
    }
}

/*************************************************************************

ham - Hamming window

Scales both real and imaginary parts of input array in-place.
Requires COMPLEX pointer and length of input array.

void ham(COMPLEX *x, int n)

*************************************************************************/

void ham(x, n)
  COMPLEX    *x;
  int        n;
{
  int    i;
  double ham,factor;

  factor = 8.0*atan(1.0)/(n-1);
  for (i = 0 ; i < n ; i++){
    ham = 0.54 - 0.46*cos(factor*i);
    x->real *= ham;
    x->imag *= ham;
    x++;
  }
}

/*************************************************************************

han - Hanning window

Scales both real and imaginary parts of input array in-place.
Requires COMPLEX pointer and length of input array.

void han(COMPLEX *x, int n)

*************************************************************************/

void han(x, n)
  COMPLEX    *x;
  int        n;
{
  int    i;
  double factor,han;

  factor = 8.0*atan(1.0)/(n-1);
  for (i = 0 ; i < n ; i++){
    han = 0.5 - 0.5*cos(factor*i);
    x->real *= han;
    x->imag *= han;
    x++;
  }
}

/*************************************************************************

triang - triangle window

Scales both real and imaginary parts of input array in-place.
Requires COMPLEX pointer and length of input array.

void triang(COMPLEX *,int n)

*************************************************************************/

void triang(x, n)
  COMPLEX    *x;
  int        n;
{
  int    i;
  double tri,a;
  a = 2.0/(n-1);

  for (i = 0 ; i <= (n-1)/2 ; i++) {
    tri = i*a;
    x->real *= tri;
    x->imag *= tri;
    x++;
  }
  for ( ; i < n ; i++) {
    tri = 2.0 - i*a;
    x->real *= tri;
    x->imag *= tri;
    x++;
  }
}

/*************************************************************************

black - Blackman window

Scales both real and imaginary parts of input array in-place.
Requires COMPLEX pointer and length of input array.

void black(COMPLEX *x, int n)

*************************************************************************/

void black(x, n)
  COMPLEX    *x;
  int        n;
{
  int    i;
  double black,factor;

  factor = 8.0*atan(1.0)/(n-1);
  for (i=0; i<n; ++i){
    black = 0.42 - 0.5*cos(factor*i) + 0.08*cos(2*factor*i);
    x->real *= black;
    x->imag *= black;
    x++;
  }
}

/*************************************************************************

harris - 4 term Blackman-Harris window

Scales both real and imaginary parts of input array in-place.
Requires COMPLEX pointer and length of input array.

void harris(COMPLEX *x, int n)

*************************************************************************/

void harris(x, n)
  COMPLEX    *x;
  int        n;
{
  int    i;
  double harris,factor,arg;

  factor = 8.0*atan(1.0)/n;
  for (i=0; i<n; ++i){
    arg = factor * i;
    harris = 0.35875 - 0.48829*cos(arg) + 0.14128*cos(2*arg)
               - 0.01168*cos(3*arg);
    x->real *= harris;
    x->imag *= harris;
    x++;
  }
}

/**************************************************************************

log2 - base 2 logarithm

Returns base 2 log such that i = 2**ans where ans = log2(i).
if log2(i) is between two values, the larger is returned.

int log2(unsigned int x)

*************************************************************************/

int log2(x)
    unsigned int x;
{
    unsigned int mask,i;

    if(x == 0) return(-1);      /* zero is an error, return -1 */

    x--;                        /* get the max index, x-1 */

    for(mask = 1 , i = 0 ; ; mask *= 2 , i++) {
        if(x == 0) return(i);   /* return log2 if all zero */
        x = x & (~mask);        /* AND off a bit */
    }
}
