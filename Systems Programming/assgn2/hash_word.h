typedef struct word_info word_list;
struct word_info{
  char *word;
  int count;
  word_list* next;
};
extern word_list*topN(int n);
extern void print_h(int n, word_list* new);
extern void install(char *word);
word_list* biggest_count();
int size_of_hashtab(word_list **hashtab);
