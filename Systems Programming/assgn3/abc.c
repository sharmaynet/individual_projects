#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
int main()
{
  char * a = NULL;
  int b;
  int x = 10;
  int count = 1;
  a = (char *) malloc (x * sizeof(char));
  strcpy(a, "1");
  strcat(a, "0");
  count++;
  printf("%s\n", a);
  b = atoi(a);
  printf("%i\n", b);
  return 0;
}
