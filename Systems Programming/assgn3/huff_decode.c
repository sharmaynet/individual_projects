#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "huff_decode.h"

uint8_t string_to_uint1(char* b)
{
  uint8_t bit = 0;
  int len = strlen(b);
  len = len-1;
  while (*b!='\0')
  {
    if (*b == '1')
    {
      bit = bit | (1<<len);
    }
    len--;
    b++;
  }
  return bit;
}
uint32_t string_to_uint2(char* b)
{
  uint32_t bit = 0;
  int len = strlen(b);
  len = len-1;
  while (*b!='\0')
  {
    if (*b == '1')
    {
      bit = bit | (1<<len);
    }
    len--;
    b++;
  }
  return bit;
}
char * uint_to_string(uint8_t num, char* c)
{
  char *temp = c;
  int i = 0;
  uint8_t mask;
  for(mask = 0x80; i<8; mask>>=1, i++)
  {
    if(num&mask)
    {
      temp[i] = '1';
    }
    else
    {
      temp[i] = '0';
    }
  }
  temp[8] = '\0';
  return temp;
}
/*use to test 32 bit num*/
void print_binary1(uint32_t num)
{
  uint32_t mask;
  for(mask = 0x8000; mask; mask>>=1)
  {
    if(num&mask)
    {
      putchar('1');
    }
    else
    {
      putchar('0');
    }
  }
  putchar('\n');
}
/*use to test 8 bit num*/
void print_binary2(uint8_t num)
{
  uint8_t mask;
  for(mask = 0x80; mask; mask>>=1)
  {
    if(num&mask)
    {
      putchar('1');
    }
    else
    {
      putchar('0');
    }
  }
  putchar('\n');
}
