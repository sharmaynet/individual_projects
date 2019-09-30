#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define MAXLEN 1000

/* 
 * In read_word.c, we fgetc each character in the file we are reading from, 
 * and checking whether or not it is a letter. If it a number, symbol, or 
 * special character (" ", "\n", etc.), we will stop reading from the file, 
 * indicating the end of one word. With each letter we find, we set it to 
 * its lowercase version, and store it into an address. We output 
 * the word back to the main.
 */
char *read_word(FILE *file)
{
    int c;
    char *p = NULL;
    int i = 0, x = 2;
    p = (char *) malloc((MAXLEN*x)* sizeof(char));
    if(p == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    while (((c = fgetc(file)) != EOF))
    {
        if(c == ' ' && (i == 0))
        {
            continue;
        }
        if((c == ' ') && (i != 0))
        {
            break;
        }
        if ((!(isalpha(c))) && (i == 0))
        {
            continue;
        }
        else if ((!(isalpha(c))) && (i!=0))
        {
            break;
        }
        if (isalpha(c))
        {
            p[i] = tolower(c);
        }
        i++;
        if (i > (MAXLEN+1))
        {
            p = (char *)realloc(p, (MAXLEN*x) * sizeof(char));
            if(p == NULL)
            {
                perror("fopen");
                exit(EXIT_FAILURE);
            }
            x++;
        }
    }
    if (c == EOF)
    {
        return NULL;
    }
    if (i != 0)
    {
        p[i]= '\0';
    }
    return p;
}
