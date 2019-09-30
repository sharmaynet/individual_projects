/*
Sharmayne Tanedo
detab.c
detab replaces tabs in its input with proper number of blanks
to space to the next stop.

** assume tab stops are every 8 space

take i/o (use argv and argc), expanding tabs as it goes

a tab character causes cursor to move to the next tab-stop
so a tab will move the cursor from wherever it is to the next
column that is a multiple of 8 (if you start coundting from 0,
or 8n+1 if you start at 1)

tab can expand to anywhere between 1 and 8 spaces

tricks:
  -one should not be able to backspace beyond the left margin
  -any character that changes the column could have effect on
  your tab-stop calc
  -\t \b \n \r

*/

#include <stdio.h>
#define TAB 8
int main(int argc, char *argv[])
{
  int c, index;
  index = 0; /*index needed to go back to 0 when \r*/
  while ((c = getchar()) != EOF)
  {
    if (c == '\t')
    {
      /*d\t does tab*/
      index++;
      while (index % TAB != 0)
      { /*keep going until tab-space is reached*/
        putchar(' ');
        index++;
      }
      putchar(' ');
    }
    else if (c == '\b')
    { /*backspace means index just goes one back*/
      if (index != 0)
      {
        index--;
      }
      putchar(c);
    }
    else if (c == '\n')
    { /*goes to new line*/
      index = 0;
      putchar(c);
    }
    else if (c == '\r')
    { /*return carriege characters after \r so index = 0*/
      index = 0;
      putchar(c);
    }
    else{
      index++;
      putchar(c);
    }
  }
  return 0;
}
