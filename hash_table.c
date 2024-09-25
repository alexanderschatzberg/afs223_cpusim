#include <stdlib.h>
#include "hash_table.h"

static uint64_t modulo_hash(uint64_t key)
{
  return key % size;
}

hashtable *ht_init(void)
{
  hashtable *ht = malloc(sizeof(hashtable));
  for (int i = 0; i < size; ++i)
  {
    ht->table[i] = NULL;
  }
  return ht;
}

void ht_insert(hashtable *ht, uint64_t key, uint64_t value)
{
  uint64_t index = modulo_hash(key);
  node *new_node = malloc(sizeof(node));
  new_node->key = key;
  new_node->value = value;
  new_node->next = ht->table[index];
  ht->table[index] = new_node;
}

uint64_t ht_get(hashtable *ht, uint64_t key)
{
  uint64_t index = modulo_hash(key);
  node *curr = ht->table[index];
  while (curr != NULL)
  {
    if (curr->key == key)
    {
      return curr->value;
    }
    curr = curr->next;
  }
  return 0;
}

void ht_free(hashtable *ht)
{
  for (int i = 0; i < size; ++i)
  {
    node *curr = ht->table[i];
    while (curr != NULL)
    {
      node *temp = curr;
      curr = curr->next;
      free(temp);
    }
  }
  free(ht);
}