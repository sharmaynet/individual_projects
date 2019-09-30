/* this program prompts for and reads a single mush command_line
 * and parses it into a list of comands showing the inputs,
 * outputs, and arguments of each.
 * then it pipes the number of stages, and runs
 * each child process. If interrupted, wait for child process
 * to end, but doesnt quit the parent process until it is EOF
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include "parseline.h"

struct sigaction sa;

int main(int argc, char *argv[]){
  char **stages;
  Stage *stage_arr;
  int num_stages, i;
  size_t line_len;
  char *new_line_ptr;
  char *pipe_line = NULL;
  FILE *fp = NULL;
  line_len = LINE_LEN;
  /*error message if greater than 2 arguments*/
  if (argc > MAX_ARGV){
    fprintf(stderr, "usage error");
    exit(EXIT_FAILURE);
  }
  /*use the arguments from the file*/
  else if (argc == MAX_ARGV){
    if ((fp = fopen(argv[1], "r")) == NULL){
      perror (argv[1]);
      exit(EXIT_FAILURE);
    }
  }
  /*use stdin for arguments*/
  else if (argc==1){
    fp = stdin;
  }
  /*stop when get SIGINT*/
  sa.sa_handler = catch_signal;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (0 != (sigaction(SIGINT, &sa, NULL))){
    perror("error calling sigaction");
    exit(EXIT_FAILURE);
  }
  /*this is for blocking/unblocking*/
  sigaddset(&sa.sa_mask, SIGINT);
  /*infinite whle loop until you reach EOF of the stdin*/
  pipe_line = (char *)malloc(line_len* sizeof(char));
  if(!pipe_line){
    perror("Error in calloc for pipe_line");
    exit(EXIT_FAILURE);
  }
  while (1){
    /*empty error count*/
    err = 0;
    /*getline only if we are getting arguments from stdin*/
    if ((fp == stdin) && (isatty(STDIN_FILENO)) && (isatty(STDOUT_FILENO))){
      printf("8-P ");
      clearerr(stdin);
      fflush(stdout);
    }
    /*read a line to grab the arguments */
    getline(&pipe_line, &line_len, fp);
    /*if user hits control D*/
    if (feof(fp)){
      if (fp == stdin){
        printf("\n");
      }
      break;
    }
    /*if empty argument*/
    if (!check_null_command(pipe_line)){
      continue;
    }

    /* delete the new line character at the pipe_line */
    new_line_ptr = strchr(pipe_line, '\n');
    if (!new_line_ptr){
      continue;
    }
    *new_line_ptr = '\0';
    if (pipe_line[0] == '|'){
      fprintf(stderr, "invalid null command\n");
      exit(EXIT_FAILURE);
    }
    err = 0;
    /* count the number of stages */
    num_stages = count_pipe(pipe_line) + 1;
    /* divide the pipe line into stages */
    stages = parse_pipe_line(pipe_line, num_stages);
    if (stages == NULL){
      free(stages);
      continue;
    }
    /* store information of each stage */
    stage_arr = parse_stage(stages, num_stages);
    if ((stage_arr[0].argc == 0) && (stage_arr[0].stage == NULL)){
      free(stage_arr);
      free(stages);
      continue;
    }
    if (stage_arr){
      /* empty stage checking */
      for(i = 0; i < num_stages; i++){
        if((stage_arr + i) == NULL){
          fprintf(stderr, "invalid null command\n");
          exit(EXIT_FAILURE);
        }
      }
      /*Special case: if cd, change the directory*/
      if (!strcmp(stage_arr->argv[0], "cd")) {
        if (stage_arr->argc == 1){
          fprintf(stderr, "%s: missing argument.\n", stage_arr->argv[0]);
          err++;
        }
        else if (-1 == chdir(stage_arr->argv[1])){
          fprintf(stderr, "%s: %s\n", stage_arr->argv[1], strerror(errno));
          err++;
        }
        free(stage_arr);
        free(stages);
        continue;
      }
    }
    /*if there have not been any errors, pipe*/
    if (err == 0){
      if (-1 == (sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL))){
        fprintf(stderr, "error with blocking signal");
        exit(EXIT_FAILURE);
      }
      pipe_it(stage_arr, num_stages);
    }
    /* free memory*/
    free_mem(stage_arr, num_stages);
    if (stages){
      free(stages);
    }
  }
  free(pipe_line);
  return 0;
}
/*sig handler*/
void catch_signal(int signum){
  printf("\n");
}
/*pipe and fork processes*/
void pipe_it(Stage * s, int num_stages)
{
  pid_t *child = NULL;
  int status = 0;
  int one[2];
  int two[2];
  int fdin, fdout;
  int i, j, num_pipes = num_stages - 1;
  child = (pid_t *)malloc(num_stages * sizeof(pid_t));
  if (child == NULL){
    perror("malloc child failed");
    exit(EXIT_FAILURE);
  }
  /*pipe the first one, will stay piped*/
  if (pipe(one)){
    perror("pipe error");
    exit(EXIT_FAILURE);
  }
  for (i = 0; i < num_stages; i++){
    /*child_process = 1;*/
    /*pipe the second one, but pipe new one every time*/
    if (pipe(two)){
      perror("pipe error");
      exit(EXIT_FAILURE);
    }
    if (! (child[i] = fork())){
      /*child i stuff*/
      /*first child*/
      if (i == 0){
        /*if there is only one pipe*/
        if (s[i].input != NULL){
          if (-1 == (fdin = open(s[i].input, O_RDONLY))){
            fprintf(stderr, "%s: %s\n", s[i].input, strerror(errno));
            exit(EXIT_FAILURE);
          }
          if (-1 == dup2(fdin, STDIN_FILENO)){
            perror("first dup2 fdin");
            exit(EXIT_FAILURE);
          }
        }
        if (num_stages == 1){
          /*no pipes - use stdin/stdout*/
          /*if output exists*/
          if (s[i].output != NULL){
            if (-1 == (fdout = open(s[i].output, O_RDWR|O_TRUNC|O_CREAT, S_IRUSR| S_IWUSR))){
              fprintf(stderr, "%s: %s\n", s[i].output, strerror(errno));
              exit(EXIT_FAILURE);
            }
            if (-1 == dup2(fdout, STDOUT_FILENO)){
              perror("first dup2 fdout");
              exit(EXIT_FAILURE);
            }
          }
        }
        else
        {
          /*use stdin/two*/
          if (-1 == dup2(two[WRITE], STDOUT_FILENO)){
            perror("first dup2 write");
            exit(EXIT_FAILURE);
          }
        }
        close(one[READ]);
        close(one[WRITE]);
        close(two[READ]);
        close(two[WRITE]);
      }
      /*last child*/
      else if (i == num_pipes){
        /*use one/stdout*/
        if (-1 == dup2(one[READ], STDIN_FILENO)){
          perror("end dup2 read");
          exit(EXIT_FAILURE);
        }
        if (s[i].output != NULL){
          if (-1 == (fdout = open(s[i].output, O_RDWR|O_TRUNC|O_CREAT, S_IRUSR| S_IWUSR))){
            fprintf(stderr, "%s: %s\n", s[i].output, strerror(errno));
            exit(EXIT_FAILURE);
          }
          if (-1 == dup2(fdout, STDOUT_FILENO)){
            perror("first dup2 fdout");
            exit(EXIT_FAILURE);
          }
        }
        close(one[READ]);
        close(one[WRITE]);
        close(two[READ]);
        close(two[WRITE]);
      }
      /*middle children*/
      else{
        /*use one/two */
        if (-1 == dup2(one[READ], STDIN_FILENO)){
          perror("middle dup2 read");
          exit(EXIT_FAILURE);
        }
        if (-1 == dup2(two[WRITE], STDOUT_FILENO)){
          perror("middle dup2 write");
          exit(EXIT_FAILURE);
        }
        close(one[READ]);
        close(one[WRITE]);
        close(two[READ]);
        close(two[WRITE]);
      }
      if (-1 == (sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL))){
        fprintf(stderr, "signal has error unblocking\n");
        exit(EXIT_FAILURE);
      }
      /*call the argument*/
      execvp(s[i].argv[0], s[i].argv);
      fprintf(stderr, "%s: %s\n", s[i].argv[0], strerror(errno));
      exit(EXIT_FAILURE);
    }
    /*parent*/
    close(one[READ]);
    close(one[WRITE]);
    /*save whatever is in the file descripter in two to one*/
    dup2(two[READ], one[READ]);
    dup2(two[WRITE], one[WRITE]);
    close(two[WRITE]);
    close(two[READ]);
  }
  /*loop to wait for all num_stages*/
  if (-1 == (sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL))){
    fprintf(stderr, "error with unblocking signal");
    exit(EXIT_FAILURE);
  }
  j = 0;
  /*wait for all children to finish*/
  while(j < num_stages){
    if (-1 == (wait(&status))){
      continue;
    }
    j++;
  }
  free(child);
}
