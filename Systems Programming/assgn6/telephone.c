#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define WRITE_END 1
#define READ_END 0
#define MSG "Boo\n"

void usage(char *name){
  fprintf(stderr,"usage: %s <stages>\n", name);
  exit(EXIT_FAILURE);
}

void telephone(int id) {
  int c;
  printf("This is process %d (pid: %d).\n",id,getpid());
  while ((EOF != (c=getchar())) )
    putchar(c);
}
int main(int argc, char *argv[]) {
  int num,i;
  char *end;
  int old[2], next[2];
  pid_t child;
  if ( argc != 2 )
    usage(argv[0]);
  num = strtol(argv[1],&end,0);
  if (num <= 0 || *end)
    usage(argv[0]);
  if ( pipe(old) ) {
    perror("old pipe");
    exit(EXIT_FAILURE);
  }
  write(old[WRITE_END],MSG,strlen(MSG));
  for(i=0; i<num; i++) {
    if ( i < num-1 ) { /* create new pipe */
      if ( pipe(next) ) {
        perror("next pipe");
        exit(EXIT_FAILURE);
      }
    }
    if ( !(child = fork()) ) {
      /* child */
      if( -1 == dup2(old[READ_END], STDIN_FILENO) ) {
        perror("dup2 old");
      }
      if ( i < num-1 ) {
        if ( -1 == dup2(next[WRITE_END], STDOUT_FILENO)) {
          perror("dup2 new");
        }
      }
      close(old[0]); /* clean house */
      close(old[1]);
      close(next[0]);
      close(next[1]);
      telephone(i);
      exit(EXIT_SUCCESS);
    }
    /* parent */
    /* close up old pipe */
    close(old[0]);
    close(old[1]);
    old[0]=next[0];
    old[1]=next[1];
  }
  while ( num--) {
    if ( -1 == wait(NULL)) {
      perror("wait");
    }
  }
  return 0;
}
