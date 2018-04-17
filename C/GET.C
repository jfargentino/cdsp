#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/**************************************************************************

GET.C - Source code for user input functions

    get_string      get string from user with prompt
    get_int         get integer from user with prompt and range
    get_float       get float from user with prompt and range

*************************************************************************/

/**************************************************************************

get_string - get string from user with prompt

Return pointer to string of input text, prompts user with string
passed by caller.  Indicates error if string space could not be
allocated.  Limited to 80 char input.

char *get_string(char *prompt_string)

    prompt_string  string to prompt user for input

*************************************************************************/

char *get_string(title_string)
    char *title_string;
{
    char *alpha;                            /* result string pointer */

    alpha = (char *) malloc(80);
    if(!alpha) {
        printf("\nString allocation error in get_string\n");
        exit(1);
    }
    printf("\nEnter %s : ",title_string);
    gets(alpha);

    return(alpha);
}
/**************************************************************************

get_int - get integer from user with prompt and range

Return integer of input text, prompts user with prompt string
and range of values (upper and lower limits) passed by caller.

int get_int(char *title_string,int low_limit,int up_limit)

    title_string  string to prompt user for input
    low_limit     lower limit of acceptable input (int)
    up_limit      upper limit of acceptable input (int)

*************************************************************************/

int get_int(title_string,low_limit,up_limit)
    char *title_string;
    int low_limit,up_limit;
{
    int i,error_flag;
    char *get_string();             /* get string routine */
    char *cp,*endcp;                /* char pointer */
    char *stemp;                    /* temp string */

/* check for limit error, low may equal high but not greater */
    if(low_limit > up_limit) {
        printf("\nLimit error, lower > upper\n");
        exit(1);
    }

/* make prompt string */
    stemp = (char *) malloc(strlen(title_string) + 60);
    if(!stemp) {
        printf("\nString allocation error in get_int\n");
        exit(1);
    }
    sprintf(stemp,"%s [%d...%d]",title_string,low_limit,up_limit);

/* get the string and make sure i is in range and valid */
    do {
        cp = get_string(stemp);
        i = (int) strtol(cp,&endcp,10);
        error_flag = (cp == endcp) || (*endcp != '\0'); /* detect errors */
        free(cp);                                   /* free string space */
    } while(i < low_limit || i > up_limit || error_flag);

/* free temp string and return result */
    free(stemp);
    return(i);
}
/**************************************************************************

get_float - get float from user with prompt and range

Return double of input text, prompts user with prompt string
and range of values (upper and lower limits) passed by caller.

double get_float(char *title_string,double low_limit,double up_limit)

    title_string  string to prompt user for input
    low_limit     lower limit of acceptable input (double)
    up_limit      upper limit of acceptable input (double)

*************************************************************************/

double get_float(title_string,low_limit,up_limit)
    char *title_string;
    double low_limit,up_limit;
{
    double x;
    int error_flag;
    char *get_string();             /* get string routine */
    char *cp,*endcp;                /* char pointer */
    char *stemp;                    /* temp string */

/* check for limit error, low may equal high but not greater */
    if(low_limit > up_limit) {
        printf("\nLimit error, lower > upper\n");
        exit(1);
    }

/* make prompt string */
    stemp = (char *) malloc(strlen(title_string) + 80);
    if(!stemp) {
        printf("\nString allocation error in get_float\n");
        exit(1);
    }

    sprintf(stemp,"%s [%1.2g...%1.2g]",title_string,low_limit,up_limit);
    
/* get the string and make sure x is in range */
    do {
        cp = get_string(stemp);
        x = strtod(cp,&endcp);
        error_flag = (cp == endcp) || (*endcp != '\0'); /* detect errors */
        free(cp);                                   /* free string space */
    } while(x < low_limit || x > up_limit || error_flag);

/* free temp string and return result */
    free(stemp);
    return(x);
}
