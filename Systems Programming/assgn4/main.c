#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>
#include <arpa/inet.h>

#include "mytar.h"

int main(int argc, char *argv[])
{
  char **files = NULL;
  int flags, i, program_status, len = 0;
  verbose = 0;
  strict = 0;
  file = 0;
  if (argc > INPUT_IDX){
    files = (char **) malloc ((argc-INPUT_IDX)* sizeof(char *));
    if (!files){
      perror("malloc files error\n");
    }
  }
  if ((argc < TAR_IDX)){
    perror("no arguments");
    exit(EXIT_FAILURE);
  }
  flags = strlen(argv[ARG_IDX]);
  for (i = 0; i<flags; i++){
    if ((argv[ARG_IDX][i]) == 'c'){
      program_status = CREATE;
    }
    else if (argv[ARG_IDX][i] == 't'){
      program_status = LIST;
    }
    else if (argv[ARG_IDX][i] == 'x'){
      program_status = EXTRACT;
    }
    else if (argv[ARG_IDX][i] == 'v'){
      verbose = 1;
    }
    else if (argv[ARG_IDX][i] == 'S'){
     strict = 1;
    }
    else if (argv[ARG_IDX][i] == 'f'){
     file = 1;
    }
    else{
      fprintf(stderr, "%s\n",
        "usage: mytar [ctxSp[f tarfile]] [file1 [ file2 [...] ] ]\n");
      exit(1);
    }
  }
  /*functions*/
  /*i is length of files*/
  if ((file == ARG_IDX) && (argc < INPUT_IDX)){
    perror("f switch with no filename");
  }
  if (program_status == CREATE){
    for(i = INPUT_IDX; i < argc; i++){
      create_tar(argv[TAR_IDX], argv[i], i, argc);
    }
  }
  if (program_status == LIST){
    if (argc>INPUT_IDX){
      for(i = INPUT_IDX; i < argc; i++){
        files[len]= argv[i];
        len++;
        }
        list_tar(argv[TAR_IDX], files, len);
    }else{
      list_tar(argv[TAR_IDX], NULL, 0);
    }
  }
  if (program_status == EXTRACT){
    if(argc > INPUT_IDX){
      for(i = INPUT_IDX; i < argc; i++){
        extract_tar(argv[TAR_IDX], argv[i]);
      }
    }
    if(argc == INPUT_IDX){
      extract_tar(argv[TAR_IDX], NULL);
    }
  }
  free(files);
  return 0;
}
