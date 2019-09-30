/*In fw.c, we call our int main(). This is where we tie in read_word.c 
 * and hash_word.c. We have all of our filters and error messages that
 *44 make our syntax work.
*/

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "read_word.h"
#include "hash_word.h"
#define DEFAULT 10

int main(int argc, char *argv[])
{
    FILE *infile = NULL;
    char *p = NULL;
    word_list * wl = NULL;
    char *end;
    int n = DEFAULT;
    int i = 1;
    if (argc > 1)
    {
        if (strcmp(argv[1], "-n") == 0)
        {
            if(argc >= 3)
            {
                n = strtol(argv[2], &end, 0);
                if(*end == '\0')
                {
                    if(n < 0)
                    {
                        fprintf(stderr, 
                    "%s: It is not possible to count fewer than zero words\n"\
                        , argv[0]);
                        exit(EXIT_FAILURE);
                    }
                    else
                    {
                        i  = 3;
                        if(argc == 3)
                        {
                            infile = stdin;
                            while((p = read_word(infile)) != NULL)
                            {
                            install(p);
                            }
                        } 
                    }
                }
                else
                {
                    fprintf(stderr,
                    "%s: not a well-formed number\n", argv[2]);
                    fprintf(stderr, 
                    "usage: fw [-n num] [ file1 [ file 2 ...] ]\n");
                    exit(EXIT_FAILURE);
                }
            }
            if(argc == 2)
            {
                fprintf(stderr, 
                "usage: fw [-n num] [ file1 [ file 2 ...] ]\n");
                exit(EXIT_FAILURE);
            }
        }
        for (; i<argc; i++)
        {
            infile = fopen(argv[i], "r");
            if (infile == NULL){
                perror(argv[i]);
                continue;
            }
            while((p = read_word(infile)) != NULL)
            {
                install(p);
            }
            fclose(infile);
        }
    }
    if (argc == 1)
    {
        infile = stdin;
        while((p = read_word(infile)) != NULL)
        {
            install(p);
        }
    }
    wl = topN(n);
    print_h(n, wl);
    free(p);
    return 0;
}
