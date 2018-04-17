
/* FILTER INFORMATION STRUCTURE FOR FILTER ROUTINES */

typedef struct {
    unsigned int length;       /* size of filter */
    float *history;            /* pointer to history in filter */
    float *coef;               /* pointer to coefficients of filter */
} FILTER;

/* FILTERS: 3 FIR AND 2 IIR */

/* 35 point lowpass FIR filter cutoff at 0.2 */

  float  fir_lpf35[35] = {
  -6.849167e-003, 1.949014e-003, 1.309874e-002, 1.100677e-002,
  -6.661435e-003,-1.321869e-002, 6.819504e-003, 2.292400e-002,
   7.732160e-004,-3.153488e-002,-1.384843e-002, 4.054618e-002,
   3.841148e-002,-4.790497e-002,-8.973017e-002, 5.285565e-002,
   3.126515e-001, 4.454146e-001, 3.126515e-001, 5.285565e-002,
  -8.973017e-002,-4.790497e-002, 3.841148e-002, 4.054618e-002,
  -1.384843e-002,-3.153488e-002, 7.732160e-004, 2.292400e-002,
   6.819504e-003,-1.321869e-002,-6.661435e-003, 1.100677e-002,
   1.309874e-002, 1.949014e-003,-6.849167e-003
                          };

    FILTER fir_lpf = {
        35,
        NULL,
        fir_lpf35
    };

/* 35 point highpass FIR filter cutoff at 0.3 same as fir_lpf35
except that every other coefficient has a different sign */

  float  fir_hpf35[35] = {
   6.849167e-003, 1.949014e-003,-1.309874e-002, 1.100677e-002,
   6.661435e-003,-1.321869e-002,-6.819504e-003, 2.292400e-002,
  -7.732160e-004,-3.153488e-002, 1.384843e-002, 4.054618e-002,
  -3.841148e-002,-4.790497e-002, 8.973017e-002, 5.285565e-002,
  -3.126515e-001, 4.454146e-001,-3.126515e-001, 5.285565e-002,
   8.973017e-002,-4.790497e-002,-3.841148e-002, 4.054618e-002,
   1.384843e-002,-3.153488e-002,-7.732160e-004, 2.292400e-002,
  -6.819504e-003,-1.321869e-002, 6.661435e-003, 1.100677e-002,
  -1.309874e-002, 1.949014e-003, 6.849167e-003
                          };

    FILTER fir_hpf = {
        35,
        NULL,
        fir_hpf35
    };

/* 52 point bandpass matched FIR filter for pulse demo */

  float  fir_pulse52[52] = {
    -1.2579e-002, 2.6513e-002,-2.8456e-016,-5.8760e-002,
     7.7212e-002,-1.4313e-015,-1.1906e-001, 1.4253e-001,
    -3.7952e-015,-1.9465e-001, 2.2328e-001,-7.7489e-015,
    -2.8546e-001, 3.1886e-001,-7.8037e-015,-3.8970e-001,
     4.2685e-001,-6.3138e-015,-5.0365e-001, 5.4285e-001,
    -1.2521e-014,-6.2157e-001, 6.6052e-001,-9.0928e-015,
    -7.3609e-001, 7.7207e-001,-3.6507e-015,-8.3886e-001,
     8.6905e-001,-1.1165e-014,-9.2156e-001, 9.4336e-001,
    -3.8072e-015,-9.7694e-001, 9.8838e-001,-1.1836e-014,
    -9.9994e-001, 9.9994e-001,-3.3578e-015,-9.5304e-001,
     8.9670e-001,-9.3099e-015,-7.3609e-001, 6.4111e-001,
    -5.9892e-015,-4.4578e-001, 3.5365e-001,-2.8959e-015,
    -1.9465e-001, 1.3058e-001,-1.7477e-016,-3.4013e-002
                          };

    FILTER fir_pulse = {
        52,
        NULL,
        fir_pulse52
    };

/* IIR lowpass 3 section (5th order) elliptic filter
with 0.28 dB passband ripple and 40 dB stopband attenuation.
The cutoff frequency is 0.25*fs. */

    float iir_lpf5[13] = {
          0.0552961603,
         -0.4363630712,  0.0000000000,  1.0000000000,  0.0000000000,
         -0.5233039260,  0.8604439497,  0.7039934993,  1.0000000000,
         -0.6965782046,  0.4860509932, -0.0103216320,  1.0000000000
        };

    FILTER iir_lpf = {
        3,                  /* 3 sections */
        NULL,
        iir_lpf5
    };

/* IIR highpass 3 section (6th order) chebyshev filter
with 1 dB passband ripple and cutoff frequency of 0.3*fs. */

    float iir_hpf6[13] = {
        0.0025892381,
        0.5913599133, 0.8879900575,-2.0000000000, 1.0000000000,
        0.9156184793, 0.6796731949,-2.0000000000, 1.0000000000,
        1.3316441774, 0.5193183422,-2.0000000000, 1.0000000000
        };

    FILTER iir_hpf = {
        3,                  /* 3 sections */
        NULL,
        iir_hpf6
    };

/* filter function prototypes */

    extern void fir_filter_array(float *,float *,int,FILTER *);
    extern void fir_filter_int(int *,int *,int,int *,int,int);

/* note: if you have a compiler that passes floats then change
         the two double declarations to float.  This problem is
         a result of using the old style function declarations. */

    extern float fir_filter(double,FILTER *);
    extern float iir_filter(double,FILTER *);
