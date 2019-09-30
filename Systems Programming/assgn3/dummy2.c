#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 4096
int main(int argc, char *argv[])
{
    int fdin, fdout;
    char buff[14];
    if(-1 == (fdin = open("myfile3.txt", O_CREAT | O_WRONLY, S_IRUSR |
      S_IWUSR)))
    {
      printf("error\n");
      exit(EXIT_FAILURE);
    }
    write(fdin, "Hello World!\n", 14);
    close(fdin);

    fdin = open ("myfile3.txt", O_RDONLY);
    if (fdin == -1)
    {
      printf("Failed to open and read the file.\n");
      exit (1);
    }
    read(fdin, buff, 14);
    buff[13] = '\0';
    printf("%s", buff);
    close(fdin);
    return 0;
    /*
    if(-1 == (fdout = open(argv[2], O_WRONLY| O_CREAT| O_TRUNC)))
    {
      perror();
      exit();
    }
    while((num=read(fdin,buff, SIZE))>0)
    {
      if (num!= write(fdout, buff, num))
      {
        if (num!= -1)
      }
    }*/
}
