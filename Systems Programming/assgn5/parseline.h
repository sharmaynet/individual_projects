typedef struct stage Stage;
struct stage{
  char *stage;
  char *input;
  char *output;
  int argc;
  char **argv;
};

void print_stages(Stage *stage_arr, int num_stages);
int count_pipe(char *pipe_line);
char **parse_pipe_line(char *pipe_line, int num_stages);
Stage *parse_stage(char **stages, int num_stages);
Stage help_parse_stage(Stage stage, char *command);
void print_input_output(Stage *stage_arr, int i, int num_stages);
void free_mem(Stage *stage_arr, int num_stages);
void check_null_command(char* token);
