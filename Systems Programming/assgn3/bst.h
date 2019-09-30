/*in bst.h, I combined a binary search tree structure
 *with a linked list structure
 *node keeps tract of
    *the count/frequency of a character
    *the ascii value of the character
    *the bit value
    *node *next for the sorted link list
    *node *right for the binary search tree
    *node *left for the binary search tree
 *I also made arr[256], arr2[256], and htree global*/

struct node
{
  uint32_t count;
  uint8_t character;
  char * bits;
  struct node *next;
  struct node *left;
  struct node *right;
};
extern int arr[256];
extern struct node *arr2[256];
extern struct node *htree;
extern void put_to_arr(uint8_t c);
extern void init_arr();
extern void put_in_bits();
extern void print_bits();
extern uint32_t count_tree();
extern void free_node(struct node *node);
