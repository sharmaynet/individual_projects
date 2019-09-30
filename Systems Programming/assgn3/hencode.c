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

/*in ./hencode, I read file1 and file2 through unix IO open(), and
 *then used read() to get the frequency of ascii values to arr[256],
 *and then stored it into the huffman tree through put_in_bits()

 *file1.txt is the infile
 *file2.huff is the outfile

 *after reading file1, I used write() to write the number of different
 *ascii values in the file as a 32 bit integer, and then I wrote each
 *character as a 8 bit integer followed by a 32 bit integer for their
 *corresponding frequency

 *I used lseek to read again from the beginning of the file. I
 *read each character one by one and outputted the corresponding
 *bits to buff1[SIZE], and then output buff1[SIZE] to file2.

 *if the last character does not fill 8 bits, then I padded the 8 bit
 *integer with 0s before adding it to buff1[SIZE]
*/
int main(int argc, char *argv[])
{
  int fdin, fdout;
  ssize_t num;
  uint32_t tree_size;
  uint8_t eight_bit;
  char *temp;
  char buff1[SIZE];
  uint8_t bytes_a[SIZE];
  int i = 0, check = 7, j = 0;
  /*open the first file*/
  if(-1 == (fdin = open(argv[1], O_RDONLY)))
  {
    printf("error with in file\n");
    exit(EXIT_FAILURE);
  }
  /*read the file and increase the count of the corresponding ascii value
   *in the arr[256]*/
  while (0 < (num = read(fdin, buff1, SIZE)))
  {
    for (i = 0;i<num;i++)
    {
      put_to_arr((uint8_t)buff1[i]);
    }
  }
  if(-1 == num)
  {
    perror("error with reading\n");
    exit(EXIT_FAILURE);
  }
  /*create the huffman tree*/
  put_in_bits();
  /*---------------------------------------------------------
        set fdin back to the beginning of the infile
  ----------------------------------------------------------*/
  if (-1 == (lseek(fdin, 0,SEEK_SET)))
  {
    perror("error with lseek");
    exit(EXIT_FAILURE);
  }
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
  else
  /*write it to stdout*/
  {
    fdout = STDOUT_FILENO;
  }
  /*find how many different characters exist in the file and write it*/
  tree_size = count_tree();
  /*---------------------------------------------------------
                            Header
  ----------------------------------------------------------*/
  if(-1 == write(fdout, &tree_size ,sizeof(uint32_t)))
  {
    perror("error with writing the count\n");
    exit(EXIT_FAILURE);
  }
  /*write the 8 bit ascii character and then 32 bit count*/
  for (i = 0; i<256; i++)
  {
    if (arr2[i]!= NULL)
    {
      if(-1 == write(fdout, &arr2[i]->character, sizeof(uint8_t)))
      {
        perror("error with writing the 8 bit character\n");
        exit(EXIT_FAILURE);
      }
      if(-1 == write(fdout, &arr2[i]->count, sizeof(uint32_t)))
      {
        perror("error with writing the 32 bit count of charactaer\n");
        exit(EXIT_FAILURE);
      }
    }
  }
  eight_bit = 0;
  /*---------------------------------------------------------
                           Body
  ----------------------------------------------------------*/
  while (0 < (num = read(fdin, buff1, SIZE)))
  {
  for (i = 0;i<num;i++)
  {
    if ((arr2[(uint8_t)buff1[i]]) && (arr2[(uint8_t)buff1[i]]->bits))
    {
      /*store the current character's bits*/
      temp= arr2[(uint8_t)buff1[i]]->bits;
      /*while going through each bit*/
      while(*temp != '\0')
      {
        /*store the character's bit into the 8-bit int*/
        if (*temp =='1')
        {
          eight_bit = eight_bit | (1<<check);
        }
        check--;
        /*if the 8-bit int index reaches -1, then save it into bytes_a[]*/
        if (check == -1)
        {
          bytes_a[j] = eight_bit;
          eight_bit = 0;
          check = 7;
          j++;
          /*if bytes_a[] reaches its limit, write it into file2, and then
            set index of bytes_a[] back to 0*/
          if (j > SIZE)
          {
            if(-1 == write(fdout, bytes_a, sizeof(uint8_t)*j))
            {
              perror("error with writing body\n");
              exit(EXIT_FAILURE);
            }
            j = 0;
          }
        }
        temp++;
      }
    }
  }
  }
  /* do not need to pad*/
  if(check == 7)
  {
    if(-1 == write(fdout, bytes_a, sizeof(uint8_t)*(j)))
    {
      perror("error with writing the body\n");
      exit(EXIT_FAILURE);
    }
  }
  else
  /*pad the 8-bit int, and then write it */
  {
    bytes_a[j] = eight_bit;
    if(-1 == write(fdout, bytes_a, sizeof(uint8_t)*(j+1)))
    {
      perror("error with writing the body\n");
      exit(EXIT_FAILURE);
    }
  }
  if (htree)
  {
    free_node(htree); /*free the huffman tree*/
  }
  close(fdin);
  close(fdout);
  return 0;
}
