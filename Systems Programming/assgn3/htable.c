#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "bst.h"

/*
in ./htable, I read the file through fopen(), and
then store it into the arr[256] through put_to_arr()
then i create the huffman tree with put_in_bits, and
then print my results
*/
int main(int argc, char*argv[])
{
    FILE *fp;
    int c;
    fp = fopen(argv[1], "r");
    if (fp == NULL){
      perror("fopen");
      exit(EXIT_FAILURE);
    }
    init_arr();
    while((c = fgetc(fp)) != EOF)
    {
      put_to_arr(c);
    }
    put_in_bits();
    print_bits();
    fclose(fp);
    return 0;
}
