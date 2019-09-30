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
#include <math.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include "mytar.h"

/*function that
      -builds blocks of headers and context of size 512
      -pack, unpack, and verify tar hearders
      -recursive traversal of the directory tree that only
       descends into real directories
*/

/*global flags*/
int verbose = 0;
int strict = 0;
int file = 0;

/*creates a new header
 *stores an individual header with stat characteristics*/
Header *store_one_header(struct stat sb, char *path){
  Header *h = NULL;
  struct passwd *pwdpt = NULL;
  struct group *grppt = NULL;
  int last_idx;
  h = (Header *)malloc(sizeof(Header));
  if(!h){
    perror("Error in malloc header");
    exit(EXIT_FAILURE);
  }
  split_name(path, h -> name, h -> prefix);
  if(S_ISDIR(sb.st_mode) && strlen(h -> name) < NAME_LEN){
    /*last index */
    last_idx = strlen(h -> name) - 1;
    if(h -> name[last_idx] != '/'){
      strcat(h -> name, "/");
    }
  }
  h -> mode =  sb.st_mode & UMASK_OCTAL;
  h -> uid = sb.st_uid;
  h -> gid = sb.st_gid;
  if(S_ISREG(sb.st_mode)){
    h -> size = sb.st_size;
  }else{
    h->size = 0;
  }
  h -> mtime = sb.st_mtime;
  strcpy(h -> magic, MAGIC);
  strcpy(h -> version, VERSION);
  pwdpt = getpwuid(sb.st_uid);
  strncpy(h -> uname, pwdpt -> pw_name, UNAME_LEN);
  grppt = getgrgid(sb.st_gid);
  strncpy(h -> gname, grppt -> gr_name, GNAME_LEN);
  return h;
}

/*math ceiling function that rounds up */
int ceil_num(int first, int second){
  int result = first/second;
  if (first%second != 0 )
    result += 1;
  return result;
}

/*splits the name if it is over the NAME_LEN and puts
 *excess bytes in prefix*/
void split_name(char *path, char *name, char *prefix){
  int len = strlen(path);
  int split_point;
  /*not greater than NAME_LEN, so no prefix needed*/
  if(len <= NAME_LEN){
    memcpy(name, path, len + 1);
    prefix[0] = '\0';
  }else{
  /*store in prefix*/
    split_point = find_split_point(path);
    strncpy(prefix, path, split_point);
    prefix[split_point] = '\0';
    memcpy(name, path + split_point, len - split_point + 1);
  }
}

/*if path length is greater than NAME_LEN, find where to cut it*/
int find_split_point(char *path){
  int len = strlen(path);
  int i = len - NAME_LEN;
  /*assume that name is greater than NAME_LEN */
  while(path[i] != '/'){
    i++;
  }
  /*do not include the slash*/
  i++;
  return i;
}

/*count number of bytes in the header*/
unsigned int count_chksum(char *archive){
  int sum = 0;
  int i;
  for(i = 0; i < CHKSUM_OFF; i++){
    sum += (unsigned char)archive[i];
  }
  /*do not include the bytes allocated to chksum*/
  for(i = CHKSUM_OFF; i < TYPEFLAG_OFF; i++){
    sum += ' ';
  }
  for(i = TYPEFLAG_OFF; i < BLOCK; i++){
    sum += (unsigned char)archive[i];
  }
  return sum;
}

/*convert specific header numerical values to octal form*/
void convert_to_octal(char *memory, int size, int num){
  char * buff = NULL;
  int n;
  buff = (char *)calloc((1+size),sizeof(num));
  if (!buff)
  {
    perror("error malloc");
    exit(EXIT_FAILURE);
  }
  /*set rest of bits that are not initialize to 0*/
  sprintf(buff, "%0*o", size-1, num);
  /*if length of buff is less than max length of the characeteristic
   *in the header, you can just store normally to memeory pointer*/
  if (strlen(buff) < size )
  {
    memmove(memory, buff, size);
  }
  /*length of buff is greater than max length of characteristic,
   *have to use GNU's function */
  else if(strlen(buff) > size - 1){
   if(!strict){
     n = insert_special_int(memory, size, num);
     if(n){
       perror("Error in inserting special int");
       exit(EXIT_FAILURE);
     }
   }
 }
 free(buff);
}

/*pack the block of 512 with the characteristics in the Header */
void pack_header(char *archive, Header *header){
  int checksum = 0;;
  memset(archive, 0, BLOCK);
  strcpy(archive, header -> name);
  /*octal num: mode, uid, gid, size, mtime, chksum*/
  convert_to_octal(archive + MODE_OFF, (int)MODE_LEN, (int)header -> mode);
  convert_to_octal(archive + UID_OFF, (int)UID_LEN, (int)header -> uid);
  convert_to_octal(archive + GID_OFF, (int)GID_LEN, (int)header -> gid);
  convert_to_octal(archive + SIZE_OFF, (int)SIZE_LEN, (int)header -> size);
  convert_to_octal(archive + MTIME_OFF,(int) MTIME_LEN, (int)header -> mtime);
  archive[TYPEFLAG_OFF] = header -> typeflag;
  /*null terminated if null fits */
  memcpy(archive + LINK_OFF, header -> linkname, LINK_LEN);
  memcpy(archive + MAGIC_OFF, header -> magic, MAGIC_LEN);
  memcpy(archive + VERSION_OFF, header -> version, VERSION_LEN);
  memcpy(archive + UNAME_OFF, header -> uname, UNAME_LEN);
  memcpy(archive + GNAME_OFF, header -> gname, GNAME_LEN);
  memcpy(archive + PREFIX_OFF, header -> prefix, PREFIX_LEN);
  checksum = count_chksum(archive);
  convert_to_octal(archive + CHKSUM_OFF, (int)CHKSUM_LEN, checksum);
}

/*if argv[1] is 'c', create a tar file out of files and directories passed
 *and put it in argv[2] tar file
 *grab all the subdirectories and files from path make them into headers
 */
void create_tar(char *t_f, char *path, int i, int argc){
  ssize_t link_val;
  char *contents = NULL, *archive = NULL;
  char link_contents[LINK_LEN];
  Header *h = NULL;
  Block *start = NULL;
  struct stat sb;
  char null_block[ENDBLOCK], root_name[NAME_LEN];
  Block *main_block= NULL;
  int n = 0, size_f, idx, out;
  memset(null_block, 0, ENDBLOCK);
  /*if the argument is 3 from the main, that means you have to truncate file*/
  if(i == INPUT_IDX){
    if (-1== (out = open(t_f, O_RDWR | O_CREAT | O_TRUNC, PERMISSION))){
      perror("error reading");
    }
  }
  /*argument is greater than 3, so it was already truncated before, and you
   *have to append*/
  else if(i > INPUT_IDX){
    if( -1 == (out = open(t_f, O_RDWR | O_CREAT | O_APPEND, PERMISSION))){
      perror("error reading");
    }
  }
  if (-1 == lstat(path, &sb)){
    perror("error in stat in create_tar");
    if(i == argc - 1){
      /*write null*/
      if(-1 ==(write(out, null_block, ENDBLOCK))){
        perror("error with writing null");
      }
      exit(EXIT_FAILURE);
    }
  }
  else{
    /*store first file/directory to the header*/
    h = store_one_header(sb, path);
    strcpy(root_name, h->name);
    /*check type*/
    if(S_ISDIR(sb.st_mode)){
      h -> typeflag = DIRTYPE;
      /*add to block*/
      start = add_block(start, h, NULL);
      /*helper function for subdirectories and files*/
      help_create(path, start, root_name);
    }
    else if (S_ISREG(sb.st_mode)){
      h -> typeflag = REGTYPE;
      /*store the contents and allocate block memory for it*/
      contents = store_contents(path);
      start = add_block(start, h, contents);
    }
    else if (S_ISLNK(sb.st_mode)){
      h -> typeflag = SYMTYPE;
      if(sb.st_size > LINK_LEN){
        perror("Too long link_name");
        exit(EXIT_FAILURE);
      }
      /*link it*/
      if(-1 == (link_val = readlink(path, link_contents, sb.st_size))){
        perror("Error in readling a symlink");
        exit(EXIT_FAILURE);
      }else{
        strcpy(h -> linkname, link_contents);
        strcat(h -> linkname, "\0");
      }
      start = add_block(start, h, NULL);
    }
  }
  /*block memory archive created*/
  archive = (char *)malloc(BLOCK * sizeof(char));
  if(!archive){
    perror("Error in mallocating archive");
    exit(EXIT_FAILURE);
  }
  main_block = start;
  /*store into block memory archive*/
  while (main_block){
    n++;
    /*first time was already malloc. the rest have to realloc*/
    if (n> 1){
      archive = (char *)realloc(archive, BLOCK * n * sizeof(char));
      if(!archive){
        perror("Error in mallocating archive");
        exit(EXIT_FAILURE);
      }
    }
    /*store the header*/
    idx = BLOCK *(n-1);
    pack_header(archive+idx, main_block -> header);
    if(verbose == 1){
      printf("%s%s\n", main_block -> header -> prefix,
          main_block -> header -> name);
    }
    /*store the contents*/
    if(main_block->contents){
      size_f = ceil_num(main_block -> header -> size, BLOCK);
      n += size_f;
      archive = (char *)realloc(archive, BLOCK * n * sizeof(char));
      if(!archive){
        perror("Error in mallocating archive");
        exit(EXIT_FAILURE);
      }
      idx = BLOCK *(n - size_f);
      /*printf("main_block: %s", main_block -> contents);*/
      pack_contents(archive+idx, main_block -> contents, size_f);
    }
    /*printf("archive contents: %s", archive + idx);*/
    main_block = main_block -> next;
  }
  /*write to the archive*/
  if (-1 ==(write(out, archive, BLOCK * n))){
    perror("error with writing byte");
  }
  /*write two null blocks - standard .tar file */
  if(i == argc - 1){
    if(-1 ==(write(out, null_block, ENDBLOCK))){
      perror("error with writing null");
    }
  }
  free_block(start);
  close(out);
}

/*free block structure*/
void free_block(Block *start){
  Block * temp = NULL;
  while (start){
    temp = start;
    free(start->header);
    free(start->contents);
    start = temp->next;
  }
}

/*read the path for all of the files and subdirectories
 *make it into a struct header
 *preorder depth first search
  -root node, recurse on children, directory, then files*/
void help_create(char *path, Block *start, char * root_name){
  Header *h = NULL;
  DIR *dir_r= NULL;
  struct dirent *dp= NULL;
  struct stat sb;
  size_t num;
  ssize_t link_val;
  char *contents = NULL;
  char link_contents[LINK_LEN];
  char curr_direct[NAME_LEN+PREFIX_LEN+1];
  /*keep track of name*/
  strcpy(curr_direct, root_name);
  if (-1 == lstat(path, &sb)){
    perror("error in help_create");
    exit(EXIT_FAILURE);
  }
  /*check types*/
  if(S_ISDIR(sb.st_mode)){
    if ((num = chdir(path)) == -1){
      perror("error changing");
      exit(EXIT_FAILURE);
    }
    if ((dir_r = opendir("."))== NULL){
      perror("not a real path to read");
      exit(EXIT_FAILURE);
    }
    while((dp = readdir(dir_r))){
      if (-1 == lstat(dp -> d_name, &sb)){
        perror("error in reading a directory");
        exit(EXIT_FAILURE);
      }
      if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..")){
        if (curr_direct[strlen(curr_direct)-1]!= '/'){
          strcat(curr_direct, "/");
        }
        strcat(curr_direct, dp -> d_name);
        h = store_one_header(sb, curr_direct);
        if(S_ISDIR(sb.st_mode)){
          h -> typeflag = DIRTYPE;
          start = add_block(start, h, NULL);
          help_create(dp -> d_name, start, curr_direct);
          strcpy(curr_direct, root_name);
          chdir("..");
        }
        else if (S_ISREG(sb.st_mode)){
          h -> typeflag = REGTYPE;
          contents = store_contents(dp -> d_name);
          start = add_block(start, h, contents);
          strcpy(curr_direct, root_name);
        }
        else if (S_ISLNK(sb.st_mode)){
          h -> typeflag = SYMTYPE;
          if(sb.st_size > LINK_LEN){
            perror("Too long link_name");
            exit(EXIT_FAILURE);
          }
          if(-1 == (link_val = readlink(dp -> d_name,
                link_contents, sb.st_size))){
            perror("Error in readling a symlink");
            exit(EXIT_FAILURE);
          }
          else{
            strcpy(h -> linkname, link_contents);
            strcat(h -> linkname, "\0");
          }
          start = add_block(start, h, NULL);
        }
      }
    }
  }
  else{
    h = store_one_header(sb, path);
    contents = store_contents(path);
    start = add_block(start, h, contents);
  }
}

/*add the header and contents to the block structure*/
Block *add_block(Block *start, Header *header, char *contents){
  Block *new = NULL;
  Block *curr = NULL;
  new = (Block *)malloc(sizeof(Block));
  if(!new){
    perror("Error in malloc for a block");
    exit(EXIT_FAILURE);
  }
  /*initialize block*/
  new -> header = header;
  if (contents){
    new -> contents =  contents;
  }
  else{
    new->contents = NULL;
  }
  new -> next = NULL;
  /*traverse to find where to store new block*/
  if(start == NULL){
    start = new;
  }else{
    curr = start;
    while(curr -> next != NULL){
      curr = curr -> next;
    }
    curr -> next = new;
  }
  return start;
}

/*store the contents if it is a regular file*/
char *store_contents(char *path){
  int fd, num;
  char *buff, *idx;
  int n = 1;
  buff = (char *)malloc(BLOCK* sizeof(char));
  if(!buff){
    perror("Error in mallocating for a contents");
    exit(EXIT_FAILURE);
  }
  fd = open(path, O_RDONLY);
  idx = buff;
  while(0 < (num = read(fd, idx, BLOCK))){
    n++;
    buff = (char *)realloc(buff, BLOCK * n * sizeof(char));
    idx+= BLOCK;
  }
  close(fd);
  return buff;
}

/*pack the contents into block memory archive*/
void pack_contents(char *archive, char *contents, int num_block){
  int i;
  for (i = 0; i < num_block; i++){
    memset(archive + (i * BLOCK), 0, BLOCK);
    memcpy(archive + (i * BLOCK), contents + (BLOCK * i), BLOCK);
  }
}

/*remove the numerical value from octal to be able
 *to put it back into the header*/
unsigned long remove_from_octal(char *memory, int size){
  char buff[BLOCK+1];
  char *endptr = NULL;
  long num;
  strncpy(buff, memory, size);
  num = strtol(buff, &endptr, OCTAL);
  if(*endptr != '\0'){
    if (!strict){
      return extract_special_int(memory, size);
    }
  }
  return num;
}

/*list all of the contents from the .tar file
 *called from main.c*/
void list_tar(char * t_f, char **path, int len){
  int fd, n = 0, size_int, i = 0, recurse = 0;
  char buff[BLOCK], fullname[NAME_LEN+PREFIX_LEN+2];
  Header * h = NULL;
  char checkversion[VERSION_LEN], checkmagic[MAGIC_LEN];
  long s;
  n++;
  if (-1== (fd = open(t_f, O_RDONLY))){
    perror("error reading in list_tar");
  }
  /*while there is stuff to read*/
  while (0 <(read(fd, buff, BLOCK))){
    /*create the header to extract values from there*/
    h = unpack_header(buff, h);
    /*create path name with prefix and name*/
    if (h->prefix[0]!= '\0'){
      strcpy(fullname, h->prefix);
      strcat(fullname, "/");
      strcat(fullname, h->name);
    }else{
      strcpy(fullname, h->name);
    }
    memcpy(checkversion, h->version, VERSION_LEN);
    memcpy(checkmagic, h->magic, MAGIC_LEN-1);
    s = h->size;
    if ((path != NULL)){
      for (i = 0; i < len; i++){
        if (strstr(fullname, path[i])!= NULL){
          recurse = 1;
        }
      }
    }
    else if (path == NULL){
      recurse = 1;
    }
    /*if pass all the tests to recurse, print the file/directory*/
    if (recurse){
      if ((h->name[0]!='\0')){
          if ((strncmp(checkmagic, MAGIC, MAGIC_LEN-1)!= 0)
              || (h->chksum != count_chksum(buff))){
            perror("bad tar file");
            exit(EXIT_FAILURE);
            }
          if (verbose == 1){
            /*write verbose things*/
            print_permission(h);
            print_ugname(buff);
            print_size_time(h);
          }
          printf("%s\n", fullname);
      }
    }
    /*if there is a size, that means it is a file, and we should
     *lseek out of the context*/
    if (s){
      size_int = (int)s;
      size_int = ceil_num(size_int, BLOCK);
      if (-1 == (lseek(fd, size_int * BLOCK ,SEEK_CUR))){
	       perror("error with lseek");
       }
    }
    recurse = 0;
    free(h);
  }
  close(fd);
}
/* helper function for listing the permissions*/
void print_permission(Header * h){
  char flag = h->typeflag;
  long mode = h->mode;
  /*print flag type*/
  if (flag == DIRTYPE){
    printf("d");
  }
  else if (flag == SYMTYPE){
    printf("l");
  }
  else{
    printf("-");
  }
  /*for user, print the rwxrwxrwx*/
  printf( (mode & S_IRUSR) ? "r" : "-");
  printf( (mode & S_IWUSR) ? "w" : "-");
  printf( (mode & S_IXUSR) ? "x" : "-");
  printf( (mode & S_IRGRP) ? "r" : "-");
  printf( (mode & S_IWGRP) ? "w" : "-");
  printf( (mode & S_IXGRP) ? "x" : "-");
  printf( (mode & S_IROTH) ? "r" : "-");
  printf( (mode & S_IWOTH) ? "w" : "-");
  printf( (mode & S_IXOTH) ? "x" : "-");
}
/* helper function for listing the user and group name*/
void print_ugname(char *buff){
  int i;
  char u_name[UNAME_LEN];
  char g_name[GNAME_LEN];
  for (i = 0; i <UNAME_LEN; i++){
    u_name[i] = buff[i+UNAME_OFF];
  }
  for (i = 0; i <GNAME_LEN; i++){
    g_name[i] = buff[GNAME_OFF+i];
  }
  printf(" %s/%s ", u_name, g_name);
}
/* helper function for listing the size and time*/
void print_size_time(Header * h){
  time_t t;
  char m_time[MTIME_LEN];
  t = h->mtime;
  strftime(m_time,MTIME_LEN,"%Y-%m-%d", localtime(&t));
  printf("%8li %s", (long)h->size, m_time);
  strftime(m_time,MTIME_LEN,"%H:%M", localtime(&t));
  printf(" %s ", m_time);
}
/*extract tar is called from main, extract the file from tar*/
void extract_tar(char *t_f, char *names){
  Block *block;
  static int fd;
  int num;
  char buff[512];
  if (-1== (fd = open(t_f, O_RDONLY))){
    perror("error reading");
  }
  /*printf("%s\n", names);*/
  while(0 < (num = read(fd, buff, BLOCK))){
    if(*buff){
      block = unpack_tar(buff, fd, names);
      create_files(block);
    }
  }
  close(fd);
}

/*helper function that returns a block from the block memory archive*/
Block *unpack_tar(char *buff, int fd, char *names){
  Header * h = NULL;
  Block *start = NULL;
  char *context, *path_idx;
  int n = 0, recurse = 1, len;
  h = unpack_header(buff, h);
  if (names != NULL)
  {
    if (strlen(h->prefix)){
        len = strlen(h->name) +strlen( h->prefix);
    }else{
    	len = strlen(h->name);
    }
    if (len <= strlen(names)){
      path_idx = names+ len;
    }
    if ((h->typeflag == REGTYPE) && (strstr(names, h->name))
      && (strlen(path_idx)!= 0)){
        recurse = 0;
    }
    if (strlen(h->prefix)){
      if (strstr(names, h->name) && (strstr(names, h->prefix))){
			     recurse = 1;
 		  }
	  }
  }
  if ((recurse == 1) && (names == NULL || ((names != NULL)
    && ((strstr(names, h -> name)) || (strstr(h->name, names)))))){
    if (h -> typeflag ==DIRTYPE){
      start = add_block(start, h, NULL);
    }
    else if (h -> typeflag == SYMTYPE){
      start = add_block(start, h, NULL);
    }
    else if (h -> typeflag == REGTYPE){
      n = h -> size;
      n = ceil_num(n, BLOCK);
      context = (char *)malloc(BLOCK * n * sizeof(char));
      if(!context){
        perror("error in mallocating for context");
        exit(EXIT_FAILURE);
      }
      if ( -1 == read(fd, context, BLOCK * n)){
        perror("error in reading");
      }
      start = add_block(start, h, context);
    }
  }
  return start;
}

/*creates files for extract*/
void create_files(Block *block){
  int out = 0;
  struct stat sb;
  Header *h;
  char path[NAME_LEN+PREFIX_LEN+1];
  if(block == NULL){
    return;
  }
  h = block -> header;
  if (strlen(h -> prefix)){
    strcpy(path, h->prefix);
    strcat(path, "/");
    strcat(path, h->name);
  }
  else if (strlen(h->name)){
    strcpy(path, h->name);
  }
  else{
     strcpy(path, "\0");
  }
  if (path[0]!= '\0'){
    if (h -> typeflag == DIRTYPE){
      if(verbose == 1){
	       printf("%s\n", h -> name);
      }
      if(-1 == (lstat(path, &sb))){
        if (-1 == mkdir(path, PERMISSION)){
          perror("error mkdir");
        }
        lstat(path, &sb);
      }
      sb.st_mode = h -> mode;
      sb.st_uid = h -> uid;
      sb.st_gid = h -> gid;
      sb.st_size = h -> size;
      sb.st_mode = h -> mode;
      sb.st_mtime = h -> mtime;
    }
    else if (h->typeflag == SYMTYPE){
      if (-1 == symlink(h->linkname, path)){
        perror(path);
      }
    }
    else if (h->typeflag == REGTYPE){
      if (verbose == 1){
        printf("%s\n", h -> name);
      }
      if (-1 == (out = open(path, O_RDWR | O_CREAT | O_TRUNC, PERMISSION))){
        perror("error creating");
      }
      if (-1 == (fstat(out, &sb))){
        sb.st_mode = h -> mode;
        sb.st_uid = h -> uid;
        sb.st_gid = h -> gid;
        sb.st_size = h -> size;
        sb.st_mode = h -> mode;
        sb.st_mtime = h -> mtime;
      }
      if (-1 == write(out, block -> contents, h -> size)){
        perror("error writing");
      }
      close(out);
    }
  }
}

/*unpack the header from the block memory archive*/
Header *unpack_header(char * archive, Header * h){
  h = (Header *)malloc(sizeof(Header));
  if(!h){
    perror("error in malloc when unpacking");
    exit(EXIT_FAILURE);
  }
  strncpy(h->name, archive+NAME_OFF, NAME_LEN);
  h->mode = (mode_t) remove_from_octal(archive+MODE_OFF, MODE_LEN);
  h->uid = (uid_t) remove_from_octal(archive+UID_OFF, UID_LEN);
  h->gid = (gid_t) remove_from_octal(archive+GID_OFF, GID_LEN);
  h->size = (off_t) remove_from_octal(archive+SIZE_OFF, SIZE_LEN);
  h->mtime = (time_t) remove_from_octal(archive+MTIME_OFF, MTIME_LEN);
  h->chksum = (int) remove_from_octal(archive + CHKSUM_OFF, CHKSUM_LEN);
  h->typeflag = archive[TYPEFLAG_OFF];
  strncpy(h->linkname, archive+LINK_OFF, LINK_LEN);
  memcpy(h->magic, archive+MAGIC_OFF, MAGIC_LEN);
  strncpy(h->version, archive+VERSION_OFF, VERSION_LEN);
  memcpy(h->uname, archive+UNAME_OFF, UNAME_LEN);
  memcpy(h->gname, archive+GNAME_OFF, GNAME_LEN);
  h->devmajor = remove_from_octal(archive+DEVMAJOR_OFF, DEVMAJOR_LEN);
  h->devminor = remove_from_octal(archive+DEVMINOR_OFF, DEVMINOR_LEN);
  strncpy(h->prefix, archive+PREFIX_OFF, PREFIX_LEN);
  return h;
}

/*extract and insert for special ints when have strict flag*/
uint32_t extract_special_int(char *where, int len){
  /*for interoperability with GNI tar. GNI seems to
   *set the high-order bit of the first byte, then treat the
   *treat the rest of the field as a binary integer
   *in network byte order.
   *I don't know for sure if it's a 32 or 64-bit int, but for
   *this version, well only support 32.
   *returns the integer on success, -1 on failure
   *in spite of the name of htonl(), it converts int32_t
   */
   int32_t val = -1;
   if((len >= sizeof(val)) && (where[0] & 0x80)){
     /*the top bit is set and we have space
      *extract the last four bytes */
     val = *(int32_t *)(where+len-sizeof(val));
     val = ntohl(val);  /*convert to host byte order*/
   }
   return val;
}

int insert_special_int(char *where, size_t size, int32_t val){
  /* For interoperability with GNU tar. GNU seems to
   * set the high–order bit of the first byte, then
   * treat the rest of the field as a binary integer
   * in network byte order.
   * Insert the given integer into the given field
   * using this technique. Returns 0 on success, nonzero * otherwise
   */
  int err = 0;
  if (val < 0 || (size < sizeof(val))){
    /* if it’s negative, bit 31 is set and we can’t use the flag
     * if len is too small, we can’t write it. * done.
     */
    err++;
  }else{
    /*game on.....*/
    memset(where, 0, size);   /*clear out the buffer*/
    *(int32_t *)(where+size - sizeof(val)) = htonl(val);
    *where |= 0x80;           /*set that high-order bit*/
  }
  return err;
}
