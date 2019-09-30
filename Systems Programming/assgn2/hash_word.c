#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include "hash_word.h"
#define WORD_SIZE 10
#define HASHSIZE 50000

/*
 *In hash_word.c, we create char ** hashtab, which is our main hash of the 
 *file we are reading. unsigned hash(char*s) helps us place the word into 
 *an index in the *hash[i], which is dependant on the length of the word. 
 *void null_hashtab() creates a new hashtab with everything NULL, 
 *so we can start recursing through it. void install(char *word) 
 *lets us put each word from the file into the hashmap, indexes the word, 
 *and if there is another word already in that index, appends them together. 
 *word_list *topN (int n) finds the n most frequent words, and places them 
 *into a new hashtable (separate from the main hashtab). 
 *word_list *biggest_count() is a helper function for topN, which recurses 
 *through the hash and finds the max count. void print_h(int n, word_list *new)
 *prints the message that will appear when we call ./fw, followed by the n 
 *words. int size_of_hashtab(word_list **hashtab) returns the number of 
 *different words in the hash.
*/

static word_list **hashtab = NULL;

/*hash: form hash value for string s */
unsigned hash(char *s)
{
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
    {
        hashval = *s + 31 * hashval;
    }
    return hashval % HASHSIZE;
}

/* null_hashtab: make all the values in the hashtab NULL */
void null_hashtab()
{
    int i;
    hashtab = (word_list **)malloc(HASHSIZE * sizeof(word_list *));
    if(!hashtab)
    {
        perror("Nothing in word_list");
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < HASHSIZE; i++)
    {
        hashtab[i] = NULL;
    }
}

/* install: put word in hashtab */
void install(char *word)
{
    word_list *word_pointer, *temp;
    if(hashtab == NULL)
    {
        null_hashtab();
    }
    word_pointer = hashtab[hash(word)];
    for (temp = word_pointer; temp; temp = temp -> next)
    {
        if(strcmp(word, temp -> word) == 0)
        {
            temp ->count++;
            break;
        }
    }
    if(temp == NULL)
    {
        temp = (word_list *)malloc(sizeof(word_list));
        if(temp == NULL)
        {
            perror("Error in mallocating a word");
            exit(EXIT_FAILURE);
        }
        temp -> word = word;
        temp -> count = 1;
        temp -> next = word_pointer;
        hashtab[hash(word)] = temp;
    }
}

/*topN: put topN words in a list*/
word_list *topN(int n)
{
    word_list *new;
    word_list *wlp;
    word_list *wlp2;
    new = (word_list*)malloc(n*sizeof(word_list));
    wlp2 = new;

    while (n!= 0)
    {
        wlp = biggest_count();
        if (!wlp)
        {
          return NULL;
        }
        else if(wlp -> count == 0)
        {
            break;
        }
        new -> word = wlp -> word;
        new -> count = wlp -> count;
        new++;
        n--;
    }
    return wlp2;
}

/* biggest_count: get the biggest word in the
 * hashtab based on the number of counts and
 lexographical order if the words have the same counts */
word_list *biggest_count()
{
    word_list *new;
    word_list *wlp;
    int wrd_c;
    int i = 0;
    word_list *max = NULL;
    if (hashtab == NULL)
    {
        return NULL;
    }
    for(;i!= HASHSIZE; i++)
    {
        max = hashtab[i];
        if(max)
        {
            break;
        }
    }
    for(i = 0;i != HASHSIZE; i++)
    {
        wlp = hashtab[i]; /*the address*/
        while(hashtab[i])
        {
            if(max->count < hashtab[i]->count)
            {
                max = hashtab[i];
            }
            else if(max->count == hashtab[i]->count)
            {
                wrd_c = strcmp(max->word, hashtab[i]->word);
                if (wrd_c<0)
                {
                    max = hashtab[i];
                }
            }
            if(hashtab[i] -> next)
            {
                hashtab[i] = hashtab[i] -> next;
            }
            else
            {
                break;
            }
        }
        hashtab[i]=wlp;
    }
    new = (word_list *)malloc(sizeof(word_list));
    new->count = max->count;
    new->word = max->word;
    max->count = 0;
    return new;
}

/*print_h: print the word and its count*/
void print_h(int n, word_list* new)
{
    int i;
    int s = size_of_hashtab(hashtab);
    printf("The top %d words (out of %d) are:\n", n, s);
    if (s!= 0)
    {
        for (i = 0;i<n;i++)
        {
            if (i == s)
            {
                break;
            }
            printf("%9d %s\n", new->count, new->word);
            new++;
        }
    }
    free(hashtab);
    null_hashtab();
}

/* size_of_hashtab: get the number of words assigned in hashtab */
int size_of_hashtab(word_list **hashtab)
{
    int i;
    word_list *head;
    int count = 0;
    if(hashtab)
    {
        for(i = 0; i < HASHSIZE; i++)
        {
            for(head = hashtab[i]; head != NULL; head = head -> next)
            {
                count++;
            }
        }
    }
    return count;
}
