#include <stdint.h>
#include <inttypes.h>

#define size 128

// Simple implementation of a hash table to simulate memory

typedef struct node
{
  uint64_t key;
  uint64_t value;
  struct node *next;
} node;

typedef struct
{
  node *table[size];
} hashtable;

hashtable *ht_init(void);
void ht_free(hashtable *ht);

// Relevant functions for logic.c implementation.
// There will be no need to delete elements from the table.
void ht_insert(hashtable *ht, uint64_t key, uint64_t value);
uint64_t ht_get(hashtable *ht, uint64_t key);
