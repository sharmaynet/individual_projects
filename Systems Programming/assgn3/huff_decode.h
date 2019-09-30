struct d_node
{
  uint32_t count;
  uint8_t character;
  char * bits;
};
extern void print_binary2(uint8_t num);
extern void print_binary1(uint32_t num);
struct d_node *sorted_insert_d_node(struct d_node *l, uint32_t cnt, uint8_t ch);
extern uint8_t string_to_uint1(char* b);
extern uint32_t string_to_uint2(char* b);
char * uint_to_string(uint8_t num, char* c);
