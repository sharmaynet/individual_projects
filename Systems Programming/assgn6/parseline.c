/* this program prompts for and reads a single mush command_line
 * and parses it into a list of comands showing the inputs,
 * outputs, and arguments of each.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parseline.h"


int err = 0;

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
int check_null_command(char* token){
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
    /*fprintf(stderr, "4 invalid null command\n");*/
    err++;
    return 0;
  }
  return 1;
}

/* divide a long pipe line into stages by "|" */
char **parse_pipe_line(char *pipe_line, int num_stages){
  char *token;
  int i = 0;
  char **stages = (char**)calloc(num_stages, sizeof(char*));
  if(!stages){
    perror("Error in calloc for stages");
    err++;
  }
  if(num_stages > 1){
    token = strtok(pipe_line, "|");
    /* null(empty) stage */
    if(!token){
      fprintf(stderr, "invalid null command\n");
      err++;
      free(stages);
      return NULL;
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
    err++;
  }

  /* inserting each stage to Stage structure */
  for(i = 0; i < num_stages; i++){
      command = stages[i];
      /* null(empty) stage */
      if(!command){
        fprintf(stderr, "invalid null command\n");
        err++;
        stage_arr[0].stage = NULL;
        stage_arr[0].argc = 0;
        return stage_arr;
        break;
      }

      /*need to save the command because if you token it, it will be broken*/
      command_cpy = (char *)malloc((strlen(command) + 1)* sizeof(char));
      if(!command_cpy){
        perror("Error in calloc for command_cpy");
        err++;
      }
      strcpy(command_cpy, command);

      /* store command of each stage */
      stage_arr[i].stage = command_cpy;

      /* store the data of each stage and save it into the array */
      stage = help_parse_stage(stage_arr[i], command, i);
      stage_arr[i] = stage;
      if ((stage_arr[i].stage == NULL) && (stage_arr[i].argc == 0))
      {
        stage_arr[0].stage = NULL;
        stage_arr[0].argc = 0;
        return stage_arr;
      }

      /* ambiguous error check */
      if(num_stages != 1){

        /* if the stage is not the last one, it should not have an
         * output to anywhere other than the next stage */
        if((i < num_stages - 1) && (stage_arr[i].output != NULL)){
          fprintf(stderr, "%s: ambiguous output\n", stage_arr[i].argv[0]);
          err++;
          stage_arr[0].stage = NULL;
          free(command_cpy);
          stage_arr[0].argc = 0;
          return stage_arr;
        }

        /* if the stage is not the first one, it should not have an
         * input from anywhere other than the previous stage */
        else if((i > 0) && (stage_arr[i].input != NULL)){
          fprintf(stderr, "%s: ambiguous input \n", stage_arr[i].argv[0]);
          stage_arr[0].stage = NULL;
          free(command_cpy);
          stage_arr[0].argc = 0;
          return stage_arr;
          err++;
        }
      }
    }
  return stage_arr;
}

/* store the data of each stage into our structure Stage */
Stage help_parse_stage(Stage stage, char *command, int i){
  char *token = NULL;
  char *input = NULL;
  char *output = NULL;
  int num_in_redir = 0;
  int num_out_redir = 0;
  int check_in_file = 0;
  int check_out_file = 0;
  int null_bool = 0;
  int argc = 0;
  char **argv = (char **)malloc((ARGV_LEN + 1)* sizeof(char*));
  if(!argv){
    perror("Error in calloc for argv");
    err++;
  }
  /* check if the stage is just with space */
  null_bool = check_null_command(command);
  if ((null_bool == 0) && (i != 0))
  {
    fprintf(stderr, "invalid null command\n");
    err++;
    stage.argc = 0;
    stage.stage = NULL;
    free(argv);
    return stage;
  }
  if (null_bool){
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
          err++;
          stage.argc = 0;
          stage.stage = NULL;
          free(argv);
          return stage;
        }
        else if(check_out_file == 1){
          if (argv[0]){
            fprintf(stderr, "%s: bad output redirection\n", argv[0]);
          }
          else
          {
            fprintf(stderr, ">: bad output redirection\n");
          }
          err++;
          stage.argc = 0;
          stage.stage = NULL;
          free(argv);
          return stage;
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
        err++;
        stage.argc = 0;
        stage.stage = NULL;
        free(argv);
        return stage;
      }

      else if(num_out_redir > 1){
        fprintf(stderr, "%s: bad output redirection\n", argv[0]? argv[0]: ">");
        err++;
        stage.argc = 0;
        stage.stage = NULL;
        free(argv);
        return stage;
      }

      /* checking filename after input redirection */
      if(check_in_file == 1){
        if(isspace(*token) || strpbrk(token, "<|>")){
          fprintf(stderr, "%s: bad input redirection\n", argv[0] ? argv[0]: "<" );
          err++;
          stage.argc = 0;
          stage.stage = NULL;
          free(argv);
          return stage;
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
          err++;
          stage.argc = 0;
          stage.stage = NULL;
          free(argv);
          return stage;
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
            err++;
          }
        }
        argc++;
      }
    }
    if(NULL == *argv){
      fprintf(stderr, "invalid null command\n");
      err++;
      stage.argc = 0;
      stage.stage = NULL;
      free(argv);
      return stage;
    }
    stage.input = input;
    stage.output = output;
    stage.argc = argc;
    stage.argv = argv;
  }
  return stage;
}
