#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "bst.h"
/*bst.c is the source file where I created all of my sorting
 *I can store whatever input i receive from the files to an arr[]
 *where I can sort into a node linked list, and the sort into a
 *huffman tree. After this, I assign the bits to the characters
 *depending on where they are located in the huffman tree, and
 *then printing it out
 *i also have a function to free the nodes in the end
 *to prevent any memory leaks*/
 
int arr[256]; /*stores the frequency in the beginning*/
struct node *arr2[256]; /*in order of ascii value*/
struct node *htree = NULL;/*hufftree*/

/*initializes the initial array to avoid error messages*/
void init_arr()
{
  int i=0;
  for(;i<256;i++)
  {
    arr[i] = 0;
  }
}
/*free link list structure -> this is only for testing */
void free_link(struct node *n)
{
  struct node *t = n;
  for(; t; t = n)
  {
    n = n->next;
    free(t);
  }
}

/*free the huffman tree*/
void free_node(struct node *node)
{
  if (node != NULL) {
    free_node(node->right);
    free_node(node->left);
    free(node->bits);
    free(node);
  }
}

/*adds one to the frequency whenever it appears in the txt file*/
void put_to_arr(uint8_t c)
{
  arr[c]++;
}

/*inserts a node to a sorted linked list structure
 *use node->next to traverse through the list
 *avoid the node->left and node->right*/
struct node *sorted_insert_node(struct node *l, uint32_t cnt, uint8_t ch,
  struct node *ln, struct node *rn)
{
  struct node *head, *new, *prev;
  new = (struct node *) malloc(sizeof(struct node));
  if (new == NULL)
  {
    perror("Error in mallocating new node");
    exit(EXIT_FAILURE);
  }
  /*creating the new node to add based on given parameters*/
  new-> count= cnt;
  new->character = ch;
  new->next = NULL;
  new->bits = NULL;
  new->right = rn;
  new->left = ln;
  /*if nothing is to be added to the left, add to the arr2[]*/
  if(ln == NULL)
  {
    arr2[ch] = new;
  }
  /*if the linked list is empty, return just the new node*/
  if(l == NULL)
  {
    return new;
  }
  /*if the count is less than the */
  if (cnt < l->count)
  {
    new->next = l;
    return new;
  }
  /*if the count is the same, sort by ascii character
   *if it is a supernode, place it in the front of the freq*/
  else if (cnt == l->count)
  {
    if (ch < l->character)
    {
      new ->next = l;
      return new;
    }
    else if ((ch == l->character)&&(ch == 0))
    {
      new ->next = l;
      return new;
    }
  }
  /*now loop through the linked list to find where to place the new node*/
  for (head = l;head; head = head->next)
  {
    /*found instance where cnt is less than the linked list node->count */
    if (head->count > cnt)
      {
        new->next = head;
        prev->next = new;
        break;
      }
    /*if cnt and node->count are the same, sort by ascii value*/
    else if ((head->count == cnt) && (head->character > ch))
      {
        new->next = head;
        prev->next = new;
        break;
      }
      /*sorting the supernode with another supernode
       *usually for larger files*/
    else if ((head->count == cnt)&& (head->character == ch)
      &&(head->character == 0))
    {
      new->next = head;
      prev->next = new;
      break;
    }
    /*cnt is the biggest value*/
    else if(head->next == NULL)
    {
      head->next = new;
      break;
    }
    /*set previous before changing the head to the node->next*/
    prev = head;
  }
  /*return first address of the linked list*/
  return l;
}

/*use sorted_insert_node() for everything in arr[256]
 *to store all the values*/
struct node *put_to_node(struct node *tab)
{
  int i;
  for (i = 0; i<256; i++)
  {
    if (arr[i] != 0)
    {
      tab = sorted_insert_node(tab, arr[i], i, NULL, NULL);
    }
  }
  return tab;
}

/*this will create a huffman tree out of the sorted linked list*/
void put_to_tree(struct node *curr_n)
{
  struct node *t = NULL;
  /*if the file is empty*/
  if (curr_n == NULL)
  {
    return ;
  }
  /*while there is something in the tree*/
  while(curr_n)
  {
    if (htree == NULL)
    {
      htree = curr_n;
      curr_n = curr_n ->next;
      /*if there is more than one char in file */
      if (curr_n)
      {
        /*malloc space for the binary bit count*/
        if (t == NULL)
        {
          t = (struct node *)malloc(sizeof(struct node));
          if(!t)
          {
            perror("Nothing in bst");
            exit(EXIT_FAILURE);
          }
        }
        /*set t to values*/
        t ->count = htree->count + curr_n->count;
        t ->character = 0;
        t ->right = curr_n;
        t ->left = htree;
        t -> bits = NULL;
        htree = t;
        t = NULL;
        curr_n = curr_n ->next;
      }
      /*only one char in file*/
      else
      {
        break;
      }
    }
    /*curr_n count is greater, put in right of the tree*/
    else if (htree->count <= curr_n ->count)
    {
      if (t == NULL)
      {
        t = (struct node *)malloc(sizeof(struct node));
        if(!t)
        {
          perror("Nothing in bst");
          exit(EXIT_FAILURE);
        }
      }
      /*set t to values*/
      t->left = htree;
      t->right = curr_n;
      t->character = 0;
      t->bits = NULL;
      t->count = htree->count + curr_n->count;
      htree = t;
      t = NULL;
      curr_n = curr_n->next;
    }
    /*if curr_n count is less, sort it back into the tree*/
    else if (htree->count > curr_n ->count)
    {
      curr_n = sorted_insert_node(curr_n, htree->count,
        0, htree->left, htree->right);
      /*free the htree (address) to avoid valgrind Error
       *has to do this because new address is stored for it
       *from sorted_insert_node()*/
      free(htree);
      htree = curr_n;
      curr_n = curr_n->next;
    }
  }
}

/*place the 0 to nodes on the left and 1 to nodes on the right
 *catenate the previous node's bits to the current nodes's bit
 *and add 0 or 1 accordingly*/
void count_bits(struct node * temp, int count)
{
  /*if empty file*/
  if (temp == NULL)
  {
    return ;
  }
  /*if you reach end of recursion*/
  if (((temp -> left) == NULL) && ((temp->right) == NULL))
  {
    /*if the very beginning has no bits
     *only valid for one char txt files*/
    if (temp->bits == NULL)
    {
      temp->bits = (char*)malloc((count+1)*sizeof(char));
      if (temp->bits == NULL)
      {
        perror("Nothing in bits");
        exit(EXIT_FAILURE);
      }
      strcpy(temp->bits, "\0");
    }
    else
    {
      strcat(temp->bits, "\0");
    }
    return ;
  }
  else
  {
    /*left node*/
    if (temp->left->bits == NULL)
    {
      temp->left->bits = (char*)malloc((count+1)*sizeof(char));
      if (temp->left->bits == NULL)
      {
        perror("Nothing in bits");
        exit(EXIT_FAILURE);
      }
    }
    /*the very beginning*/
    if (temp->bits == NULL)
    {
      strcpy(temp->left->bits, "0");
    }
    else
    {
      strcpy(temp->left->bits, temp->bits);
      strcat(temp->left->bits, "0");
    }
    count++;
    count_bits(temp->left, count);

    /*right node*/
    if (temp->right->bits == NULL)
    {
      temp->right->bits = (char*)malloc((count+1)*sizeof(char));
      if (temp->right->bits == NULL)
      {
        perror("Nothing in bits");
        exit(EXIT_FAILURE);
      }
    }
    /*the very beginning*/
    if (temp->bits == NULL)
    {
      strcpy(temp->right->bits, "1");
    }
    else
    {
      strcpy(temp->right->bits, temp->bits);
      strcat(temp->right->bits, "1");
    }
    count++;
    count_bits(temp->right, count);
  }
}

/*put bits into htree
 *call in main file to put all the functions in bst.c together*/
void put_in_bits()
{
  int count = 1;
  struct node * temp = NULL;
  struct node *tab = NULL;
  tab = put_to_node(tab);
  put_to_tree(tab);
  temp = htree;
  count_bits(temp, count);
}

/*print the htable, and then free the nodes*/
void print_bits()
{
  int i = 0;
  for (; i<256; i++)
  {
    if (arr[i]!= 0)
    {
        printf("0x%.2x: %s\n", i, arr2[i]->bits);
    }
  }
  if (htree)
  {
    free_node(htree);
  }
}

/*count the number of unique characters for header in encode*/
uint32_t count_tree()
{
  int i = 0;
  uint32_t count = 0;
  for (; i<256; i++)
  {
    if (arr2[i]!= NULL)
    {
      count++;
    }
  }
  return count;
}
