#define LINE_LEN 512
#define ARGV_LEN 10
#define REALLOC_LEN 2
#define READ 0
#define WRITE 1
#define MAX_ARGV 2

typedef struct stage Stage;
struct stage{
  char *stage;
  char *input;
  char *output;
  int argc;
  char **argv;
};

int err;
int count_pipe(char *pipe_line);
char **parse_pipe_line(char *pipe_line, int num_stages);
Stage *parse_stage(char **stages, int num_stages);
Stage help_parse_stage(Stage stage, char *command, int i);
void free_mem(Stage *stage_arr, int num_stages);
int check_null_command(char* token);

void catch_signal(int signum);
void pipe_it(Stage * s, int num_stages);
