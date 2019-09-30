#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "bst.h"
void print_binary(uint8_t num);
char * uint_to_string(uint8_t num, char* c);
int main()
{
  uint8_t a = 128, c = 130;
  uint8_t b = 4;
  uint8_t d;
  char *temp;
  char *temp2;
  int x =6;
  temp = (char*)malloc(sizeof(char)*18);
  temp2 = (char*)malloc(sizeof(char)*9);
  print_binary(a);
  print_binary(b);
  printf("b>>1 = %d\n", b>>1);  // The result is 00000100
  b = b>>1;
  print_binary(b>>1); /*shift to right*/
  b = b | (1<<4);
  print_binary(b);
  d = a&c;
  print_binary(d);
  d = d>>1;
  print_binary(d); /*the left most bit*/
  d = d>>6;
  print_binary(d);
  d = d>>1;
  print_binary(d);
  d = (uint8_t)x;
  temp = uint_to_string(a, temp);
  temp2 = uint_to_string(b, temp2);
  printf("%s\n", temp);
  printf("%s\n", temp2);
  strcat(temp, temp2);
  printf("%s\n", temp);
  /*int x;
    char *temp = "hello";
    int check = 7;
    uint8_t eight_bit = 3;
    uint8_t num = 0;
    num = num | (1<<4);
    printf("%i\n", (int)num);
    print_binary(num);
    x = (int) num;
    if (x == 128)
    printf("%i\n", x);
    num = 0;
    if (*temp == 'h')
      printf("%c", 'h');
    print_binary(num);
    eight_bit = 2;
    eight_bit = eight_bit | (1<<check);
    printf("%i\n", (int)eight_bit);
    print_binary(eight_bit);*/

}
void print_binary(uint8_t num)
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
  return temp;
}
