/* this program prompts for and reads a single mush command_line
 * and parses it into a list of comands showing the inputs,
 * outputs, and arguments of each.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parseline.h"

#define LINE_LEN 512
#define ARGV_LEN 10
#define REALLOC_LEN 2

int main(int argc, char *argv[]){
  char *pipe_line;
  ssize_t numchr_read;
  char **stages;
  Stage *stage_arr;
  int num_stages;
  size_t line_len;
  char *new_line_ptr;
  int i;
  line_len = LINE_LEN;
  pipe_line = (char *)calloc(line_len, sizeof(char));
  if(!pipe_line){
    perror("Error in calloc for pipe_line");
    exit(EXIT_FAILURE);
  }
  printf("line: ");
  if(-1 != (numchr_read = getline(&pipe_line, &line_len, stdin))){
    /* reads pipe_line*/;
  }

  /* delete the new line character at the pipe_line */
  new_line_ptr = strchr(pipe_line, '\n');
  *new_line_ptr = '\0';
  if (pipe_line[0] == '|')
  {
    fprintf(stderr, "invalid null command\n");
    exit(EXIT_FAILURE);
  }
  /* count the number of stages */
  num_stages = count_pipe(pipe_line) + 1;

  /* divide the pipe line into stages */
  stages = parse_pipe_line(pipe_line, num_stages);

  /* store information of each stage */
  stage_arr = parse_stage(stages, num_stages);

  /* empty stage checking */
  for(i = 0; i < num_stages; i++){
    if((stage_arr + i) == NULL){
      fprintf(stderr, "invalid null command\n");
      exit(EXIT_FAILURE);
    }
  }

  /* print info */
  print_stages(stage_arr, num_stages);

  /* free memory */
  free_mem(stage_arr, num_stages);
  free(stages);
  free(pipe_line);
  return 0;
}

/* free memory */
void free_mem(Stage *stage_arr, int num_stages){
  int i;
  /* free stage, argv, and array of stages */
  for(i = 0; i < num_stages; i++){
    free(stage_arr[i].argv);
    free(stage_arr[i].stage);
  }
  free(stage_arr);
}

/* print stored data of each stage */
void print_stages(Stage *stage_arr, int num_stages){
  int i;
  int j = 0;
  for(i = 0; i < num_stages; i++){
    printf("\n");
    printf("--------\n");
    printf("Stage %d: \"%s\"\n", i, stage_arr[i].stage);
    printf("--------\n");
    print_input_output(stage_arr, i, num_stages);
    printf("      argc: %d\n", stage_arr[i].argc);
    printf("      argv: ");
    while(j < stage_arr[i].argc){
      if(j == (stage_arr[i].argc - 1)){
        printf("\"%s\"\n", stage_arr[i].argv[j]);
      }
      else{
        printf("\"%s\",", stage_arr[i].argv[j]);
      }
      j++;
    }
    j = 0;
  }
}

/* print input and output in each stage */

void print_input_output(Stage *stage_arr, int i, int num_stages){
  if(stage_arr[i].input){
    printf("     input: %s\n", stage_arr[i].input);
  }
  else if(i == 0){
    printf("     input: %s\n", "original stdin");
  }
  else{
    printf("     input: pipe from stage %d\n", (i - 1));
  }
  if(stage_arr[i].output){
    printf("    output: %s\n", stage_arr[i].output);
  }
  else if(i == (num_stages - 1)){
    printf("    output: %s\n", "original stdout");
  }
  else{
    printf("    output: pipe to stage %d\n", (i + 1));
  }
}

/* number of stages */
int count_pipe(char *pipe_line){
  int i;
  int count = 0;
  for(i = 0; i < strlen(pipe_line); i++){
    if(pipe_line[i] == '|'){
      count++;
    }
  }
  return count;
}

/* check if stage is a space */
void check_null_command(char* token){
  int i;
  int len;
  int count = 0;
  len = strlen(token);
  for(i = 0; i < len; i++){
    if(isspace(token[i])){
      count++;
    }
  }
  if(len == count){
    fprintf(stderr, "invalid null command\n");
    exit(EXIT_FAILURE);
  }
}

/* divide a long pipe line into stages by "|" */
char **parse_pipe_line(char *pipe_line, int num_stages){
  char *token;
  int i = 0;
  char **stages = (char**)calloc(num_stages, sizeof(char*));
  if(!stages){
    perror("Error in calloc for stages");
    exit(EXIT_FAILURE);
  }
  if(num_stages > 1){
    token = strtok(pipe_line, "|");
    /* null(empty) stage */
    if(!token){
      fprintf(stderr, "invalid null command\n");
      exit(EXIT_FAILURE);
    }
    stages[i] = token;
    strcat(stages[i], "\0");
    i++;
    while(token != NULL){
      token = strtok(NULL, "|");
      if(token){
        stages[i] = token;
        strcat(stages[i], "\0");
        i++;
      }
    }
  }
  else{
    stages[i] = pipe_line;
  }
  return stages;
}

/* save info */
Stage *parse_stage(char **stages, int num_stages){
  Stage *stage_arr;
  int i;
  char *command;
  char *command_cpy;
  Stage stage;

  /* mallocing for a list of structures */
  stage_arr = (Stage *)calloc((num_stages), sizeof(Stage));
  if(!stage_arr){
    perror("Error in calloc stages");
    exit(EXIT_FAILURE);
  }

  /* inserting each stage to Stage structure */
  for(i = 0; i < num_stages; i++){
      command = stages[i];
      /* null(empty) stage */
      if(!command){
        fprintf(stderr, "invalid null command\n");
        exit(EXIT_FAILURE);
      }

      /* need to save the command because if you token it, it will be broken */
      command_cpy = (char *)calloc(strlen(command) + 1, sizeof(char));
      if(!command_cpy){
        perror("Error in calloc for command_cpy");
        exit(EXIT_FAILURE);
      }
      strcpy(command_cpy, command);

      /* store command of each stage */
      stage_arr[i].stage = command_cpy;

      /* store the data of each stage and save it into the array */
      stage = help_parse_stage(stage_arr[i], command);
      stage_arr[i] = stage;

      /* ambiguous error check */
      if(num_stages != 1){

        /* if the stage is not the last one, it should not have an
         * output to anywhere other than the next stage */
        if((i < num_stages - 1) && (stage_arr[i].output != NULL)){
          fprintf(stderr, "%s: ambiguous output\n", stage_arr[i].argv[0]);
          exit(EXIT_FAILURE);
        }

        /* if the stage is not the first one, it should not have an
         * input from anywhere other than the previous stage */
        else if((i > 0) && (stage_arr[i].input != NULL)){
          fprintf(stderr, "%s: ambiguous input \n", stage_arr[i].argv[0]);
          exit(EXIT_FAILURE);
        }
      }
    }
  return stage_arr;
}

/* store the data of each stage into our structure Stage */
Stage help_parse_stage(Stage stage, char *command){
  char *token;
  char *input = NULL;
  char *output = NULL;
  int num_in_redir = 0;
  int num_out_redir = 0;
  int check_in_file = 0;
  int check_out_file = 0;
  int argc = 0;
  char **argv = (char **)calloc(ARGV_LEN + 1, sizeof(char*));
  if(!argv){
    perror("Error in calloc for argv");
    exit(EXIT_FAILURE);
  }

  /* check if the stage is just with space */
  check_null_command(command);

  /* token it by a space */
  token = strtok(command, " ");

  /* token is not neither '<' nor '>', save it to argv */
  if(!strpbrk(token, "<>")){
    argv[argc] = token;
    strcat(argv[argc], "\0");
    /* increment the number of arguments */
    argc++;
  }

  /* if token is either '<' or '>', check the filename after */
  else{
    if(!strcmp(token, "<")){
      num_in_redir++;
      check_in_file = 1;
    }
    else if(!strcmp(token, ">")){
      num_out_redir++;
      check_out_file = 1;
    }
  }
  while(token != NULL){

    /* checking next token */
    token = strtok(NULL, " ");

    /* if there is just '<' or '>' without file name */
    if(token == NULL){
      if(check_in_file == 1){
        if (argv[0]){
          fprintf(stderr, "%s: bad input redirection\n", argv[0]);
        }
        else
        {
          fprintf(stderr, "<: bad input redirection\n");
        }
        exit(EXIT_FAILURE);
      }
      else if(check_out_file == 1){
        if (argv[0]){
          fprintf(stderr, "%s: bad output redirection\n", argv[0]);
        }
        else
        {
          fprintf(stderr, ">: bad output redirection\n");
        }
        exit(EXIT_FAILURE);
      }
      continue;
    }

    /* whether token is either '<' or '>' */
    if(!strcmp(token, "<")){
      num_in_redir++;
      check_in_file = 1;
      continue;
    }
    else if(!strcmp(token, ">")){
      num_out_redir++;
      check_out_file = 1;
      continue;
    }

    /* checking the if there is more than one redirection */
    if(num_in_redir > 1){
      fprintf(stderr, "%s: bad input redirection\n", argv[0] ? argv[0]: "<" );
      exit(EXIT_FAILURE);
    }

    else if(num_out_redir > 1){
      fprintf(stderr, "%s: bad output redirection\n", argv[0]? argv[0]: ">");
      exit(EXIT_FAILURE);
    }

    /* checking filename after input redirection */
    if(check_in_file == 1){
      if(isspace(*token) || strpbrk(token, "<|>")){
        fprintf(stderr, "%s: bad input redirection\n", argv[0] ? argv[0]: "<" );
        exit(EXIT_FAILURE);
      }
      else{
        input = token;
        check_in_file = 0;
        continue;
      }
    }

    /* checking filename after output redirection */
    else if(check_out_file == 1){
      if(isspace(*token) || strpbrk(token, "<|>")){
        fprintf(stderr, "%s: bad output redirection\n", argv[0]?argv[0]: ">");
        exit(EXIT_FAILURE);
      }
      else{
        output = token;
        check_out_file = 0;
        continue;
      }
    }

    else if((check_in_file == 0) && (check_out_file == 0)){
      argv[argc] = token;
      strcat(argv[argc], "\0");
      if(argc == ARGV_LEN - 1){
        argv = (char **)realloc(argv,
          REALLOC_LEN * (ARGV_LEN + 1) * sizeof(char*));
        if(!argv){
          perror("Error in realloc for argv");
          exit(EXIT_FAILURE);
        }
      }
      argc++;
    }
  }
  if(NULL == *argv){
    fprintf(stderr, "invalid null command\n");
    exit(EXIT_FAILURE);
  }
  stage.input = input;
  stage.output = output;
  stage.argc = argc;
  stage.argv = argv;
  return stage;
}
