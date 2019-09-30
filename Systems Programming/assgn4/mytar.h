#define NAME_LEN 100
#define NAME_OFF 0
#define MODE_LEN 8
#define MODE_OFF 100
#define UID_LEN 8
#define UID_OFF 108
#define GID_LEN 8
#define GID_OFF 116
#define SIZE_LEN 12
#define SIZE_OFF 124
#define MTIME_LEN 12
#define MTIME_OFF 136
#define CHKSUM_LEN 8
#define CHKSUM_OFF 148
#define TYPEFLAG_LEN 1
#define TYPEFLAG_OFF 156
#define LINK_LEN 100
#define LINK_OFF 157
#define MAGIC_LEN 6
#define MAGIC_OFF 257
#define VERSION_LEN 2
#define VERSION_OFF 263
#define UNAME_LEN 32
#define UNAME_OFF 265
#define GNAME_LEN 32
#define GNAME_OFF 297
#define DEVMAJOR_LEN 8
#define DEVMAJOR_OFF 329
#define DEVMINOR_LEN 8
#define DEVMINOR_OFF 337
#define PREFIX_LEN 155
#define PREFIX_OFF 345
#define BLOCK 512
#define ENDBLOCK 1024

#define UMASK_OCTAL 00007777
#define PERMISSION 0777
#define OCTAL 8
#define ARG_IDX 1
#define TAR_IDX 2
#define INPUT_IDX 3
#define CREATE 1
#define LIST 2
#define EXTRACT 3

#define MAGIC "ustar\0"
#define VERSION "00"
#define REGTYPE '0'
#define SYMTYPE '2'
#define DIRTYPE '5'

typedef struct header Header;
struct header{
  char name[NAME_LEN + 1];
  mode_t mode;
  uid_t uid;
  gid_t gid;
  off_t size;
  time_t mtime;
  unsigned int chksum;
  char typeflag;
  char linkname[LINK_LEN + 1];
  char magic[MAGIC_LEN];
  char version[VERSION_LEN];
  char uname[UNAME_LEN];
  char gname[GNAME_LEN];
  int devmajor;
  int devminor;
  char prefix[PREFIX_LEN + 1];
};

typedef struct block Block;
struct block{
  Header *header;
  char* contents;
  Block *next;
};
extern int verbose;
extern int strict;
extern int file;

Header *store_one_header(struct stat sb, char *path);
void split_name(char *path, char *name, char *prefix);
int find_split_point(char *path);
unsigned int count_chksum(char *archive);
void convert_to_octal(char *memory, int size, int num);
void pack_header(char *archive, Header *header);
void create_tar(char *t_f, char *path, int i, int argc);
void help_create(char *path, Block *start, char * root_name);
Block * add_block(Block *start, Header *header, char *contents);
char *store_contents(char *path);
void pack_contents(char *archive, char *contents, int num_block);
uint32_t extract_special_int(char *where, int len);
int insert_special_int(char *where, size_t size, int32_t val);
void list_tar(char * t_f, char **path, int len);
void print_permission(Header * h);
void print_ugname(char *buff);
void print_size_time(Header * h);
void free_block(Block *start);
void extract_tar(char *t_f, char *names);
Header *unpack_header(char * archive, Header * h);
Block *unpack_tar(char *buff, int fd, char *names);
void create_files(Block *block);
unsigned long remove_from_octal(char *memory, int size);
