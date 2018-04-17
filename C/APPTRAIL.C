#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "disk.h"
#include "get.h"

/**************************************************************************

APPTRAIL.C - PROGRAM TO APPEND MORE TEXT ON THE END OF AN EXISTING
             TRAILER.

INPUTS: FILE NAME, STRINGS TO ADD

OUTPUTS: TRAILER TEXT, UPDATED DSP DATA FILE AND TRAILER CHARACTER COUNT

*************************************************************************/

main()
{
    DSP_FILE *in;
    char *addition,*text;
    int len;

/* open the file to get the DSP_FILE structure */
    do {
        in = open_read(get_string("file name"));
    } while(!in);

/* display current trailer */
    printf("\nCurrent trailer:\n%s",read_trailer(in));

/* get first line to add to trailer */
    addition = get_string("string to add (CR to end)");
    if(strlen(addition) == 0) exit(1);   /* CR will terminate */
    strcat(addition,"\n");               /* add newline char */

/* get a series of additional strings and add on to addition */
    while(strlen(text = get_string("string")) != 0) {
        strcat(text,"\n");               /* add newline char */

/* re-allocate the addition string to add on text */
        addition = realloc(addition,
                        strlen(addition) + strlen(text) + 1);
        if(!addition) {
            printf("\nError allocating trailer space\n");
            exit(1);
        }

/* add on the new string */
        strcat(addition,text);

/* free the string made by get_string */
        free(text);
    }

/* add on to trailer using append_trailer and write_trailer */
    len = write_trailer(append_trailer(addition,in),in);

/* tell user how much text was written */
    printf("\n%d characters written\n",len);
}
