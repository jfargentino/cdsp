/* get.h - prototypes for get.c functions and common macros */

    extern char *get_string(char *);
    extern int get_int(char *,int,int);
    extern double get_float(char *,double,double);

/* MIN, MAX, ROUND macros */

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#define ROUND(a)  (((a) < 0) ? (int)((a)-0.5) : (int)((a)+0.5))
