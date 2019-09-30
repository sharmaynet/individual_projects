#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include "bst.h"
#define SIZE 4096
/*in ./hdecode, I read file1 and file2 through unix IO open*(), and
 *then use read() to get the number of different ascii values in the
 *file, the characters, and the corresponding frequency for each character
 *I then stored the results to create a huffman tree

 *file1.huff is the infile
 *file2.txt is the outfile

 *after reading the header of file1, i read the each of the following
 *8-bit integers, and traverse through the huffman tree until i reach
 *a leaf, and then i store the corresponding character of the leaf into
 *char text[SIZE]. I keep repeating this until either I reach the EOF or
 *the index of the text[SIZE] has reached its limit, where I have to
 *write the text[SIZE] into file2, and then reset the index back to 0

 *in the end, I write() the remaining text[SIZE] to file2, and then close
 *the file
*/


int main(int argc, char *argv[])
{
  int fdin, fdout, j, idx = 0;
  ssize_t num;
  uint32_t buff1[sizeof(uint32_t)];
  uint8_t buff2[SIZE], buff3[SIZE];
  uint32_t cnt, i, temp_cnt;
  uint8_t temp_ch, eight_bit, mask;
  char text[SIZE];
  struct node *tree_temp = NULL;
  /*---------------------------------------------------------
                            Header
  ----------------------------------------------------------*/
  /*open and read file1*/
  if (strcmp(argv[1], "-") == 0)
  {
    fdin = STDIN_FILENO;
  }
  else if(-1 == (fdin = open(argv[1], O_RDONLY)))
  {
    printf("error with in file\n");
    exit(EXIT_FAILURE);
  }
  num =read(fdin, buff1, sizeof(uint32_t));
  if(-1 == num)
  {
    printf("error with reading file\n");
    exit(EXIT_FAILURE);
  }

  /*how to store value in cnt without the computer screaming at me */
  cnt = buff1[0];
  /*read the next cnt amount of 8 bit character and 32 bit count,
   *and store it into the huffman tree*/
  for(i = 0; i<cnt; i++)
  {
    num = read(fdin, buff2, sizeof(uint8_t));
    if(-1 == num)
    {
      printf("error with reading file\n");
      exit(EXIT_FAILURE);
    }
    if ((0 == num) && (i < cnt))
    {
      printf("Unexpected end of file while reading encoded file\n");
      exit(EXIT_FAILURE);
    }
    temp_ch = *buff2;
    num = read(fdin, buff1, sizeof(uint32_t));
    if(-1 == num)
    {
      printf("error with reading file\n");
      exit(EXIT_FAILURE);
    }
    temp_cnt = *buff1;
    arr[(int)temp_ch] = temp_cnt;
  }
  put_in_bits();
  /*open file2*/
  if (argv[2])
  {
    if(-1 == (fdout = open(argv[2], O_WRONLY| O_CREAT | O_TRUNC,
      S_IRUSR | S_IWUSR)))
    {
      perror("error with out file\n");
      exit(EXIT_FAILURE);
    }
  }
  /*output to stdout if file2 does not exist*/
  else
  {
    fdout = STDOUT_FILENO;
  }
  /*---------------------------------------------------------
                           Body
  ----------------------------------------------------------*/

  /*if there is only one character*/
  if ((htree)&&(htree->character))
  {
    while(htree->count)
    {
      text[idx] = htree->character;
      idx++;
      htree->count--;
      /*if the idx exceeds the character array limit*/
      if (idx >= SIZE)
      {
        if(-1 == write(fdout, text, SIZE))
        {
          perror("error with writing\n");
          exit(EXIT_FAILURE);
        }
        idx = 0;
      }
    }
    /*write into the fdout*/
    if(-1 == write(fdout, text, sizeof(char)*idx))
    {
      perror("error with writing\n");
      exit(EXIT_FAILURE);
    }
    /*if this is an empty file to begin with*/
    if (htree)
    {
      free_node(htree);
    }
    close(fdin);
    close(fdout);
    return 0;
  }
  tree_temp = htree;
  /*find the character responding the bits*/
  while (0 < (num = read(fdin, buff3, SIZE)))
  {
    for (j = 0; j<num; j++)
    {
      mask = 128;
      eight_bit = buff3[j];
      while(mask)
      {
        if (eight_bit & mask) /*if the result isnt 0*/
        {
          tree_temp = tree_temp->right;
        }
        else /*result is 0*/
        {
          tree_temp = tree_temp->left;
        }
        if ((tree_temp->right == NULL) && (tree_temp->left == NULL))
        {
          /*if there is any more occurances of this character in the file*/
          if (tree_temp->count)
          {
            /*place character in string*/
            text[idx] = tree_temp->character;
            idx++;
            /*decrement the count*/
            tree_temp->count--;
            /*if the idx exceeds the character array limit*/
            if (idx >= SIZE)
            {
              if(-1 == write(fdout, text, SIZE))
              {
                perror("error with writing\n");
                exit(EXIT_FAILURE);
              }
              idx = 0;
            }
            /*set the address pointer back to the top node*/
            tree_temp = htree;
          }
          else
          {
            /*there is no more occurances of this character
             *purpose of this else statement is to account for
             *the padded 0s in the end to not count as an
             *unwanted character in the text file*/
            if(-1 == write(fdout, text, sizeof(char)*idx))
            {
              perror("error with writing\n");
              exit(EXIT_FAILURE);
            }
            if (htree)
            {
              free_node(htree);/*free the huffman tree*/
            }
            close(fdin);
            close(fdout);
            return 0;
          }
        }
        mask = mask >> 1; /*move to the right*/
      }
    }
  }
  if(-1 == num)
  {
    perror("error with reading the body\n");
    exit(EXIT_FAILURE);
  }
  /*write everything*/
  if(-1 == write(fdout, text, sizeof(char)*idx))
  {
    perror("error with writing the body\n");
    exit(EXIT_FAILURE);
  }
  if (htree)
  {
    free_node(htree); /*free the huffman tree*/
  }
  close(fdin);
  close(fdout);
  return 0;
}
