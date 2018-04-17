/* dft.h - function prototypes and structures for dft and fft functions */

/* COMPLEX STRUCTURE */

typedef struct {
    float real, imag;
} COMPLEX;

/* function prototypes for dft and inverse dft functions */

    extern void fft(COMPLEX *,int);
    extern void ifft(COMPLEX *,int);
    extern void dft(COMPLEX *,COMPLEX *,int);
    extern void idft(COMPLEX *,COMPLEX *,int);
    extern void rfft(float *,COMPLEX *,int);
    extern void ham(COMPLEX *,int);
    extern void han(COMPLEX *,int);
    extern void triang(COMPLEX *,int);
    extern void black(COMPLEX *,int);
    extern void harris(COMPLEX *,int);
    extern int log2(unsigned int);
