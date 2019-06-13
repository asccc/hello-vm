#include "tab.h"
#include "mem.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef TAB_INITIAL_SIZE
#define TAB_INITIAL_SIZE 16
#endif

/** grows the table */
static void grow_mem (struct tab *, szt);

/** generates a hash for the given name */
static u32 hash_gen (char *);

/** re-hashes the table */
static void hash_tab (struct tab *);

/** inserts a pair into the table */
static void put_pair (struct tab *, struct tab_par *);

/** creates a new entry */
static struct tab_ent * make_ent(void);

/** creates a new pair */
static struct tab_par * make_par(u32);

/** releases a pair */
static void free_par (struct tab_par *);

/** releases a entry */
static void free_ent (struct tab_ent *);

/**
 * {@inheritdoc}
 */
TAB_CALL void tab_init (struct tab *tab)
{
  assert(tab != 0);

  tab->elms = 0;
  tab->data = 0;
  grow_mem(tab, TAB_INITIAL_SIZE);
}

/**
 * {@inheritdoc}
 */
TAB_CALL void * tab_get (struct tab *tab, char *name)
{
  assert(tab != 0);
  assert(name != 0);

  struct tab_ent *item;
  struct tab_par *pair;

  u32 hash = hash_gen(name);
  u32 offs = hash & tab->mask;

  item = tab->data[offs];
  if (!item) {
    return 0;
  }

  pair = item->pair;
  while (pair) {
    if (strcmp(pair->name, name) == 0) {
      return pair->data;
    }
    pair = pair->next;
  }

  return 0;
}

/**
 * {@inheritdoc}
 */
TAB_CALL void tab_put (struct tab *tab, char *name, 
                       void *data, szt size)
{
  assert(tab != 0);
  assert(name != 0);
  assert(data != 0);
  assert(size > 0);

  struct tab_ent *item;
  struct tab_par *pair;
  struct tab_par *last = 0;

  u32 offs;
  u32 hash = hash_gen(name);

  hop:
  offs = hash & tab->mask;
  item = tab->data[offs];

  if (!item) {
    item = make_ent();
    item->next = tab->head;
    tab->head = item;
    tab->data[offs] = item;
  }

  pair = item->pair;
  while (pair) {
    if (strcmp(pair->name, name) == 0) {
      break;
    }
    last = pair;
    pair = pair->next;
  }

  if (!pair) {
    if (++tab->elms >= tab->size) {
      grow_mem(tab, tab->size << 1);
      hash_tab(tab);
      goto hop;
    }
    pair = make_par(hash);
    pair->name = mem_sdup(name);
  }

  if (pair->data) {
    free(pair->data);
  }

  pair->data = mem_aodt(size);
  memcpy(pair->data, data, size);

  if (!last) {
    item->pair = pair;
    return;
  }

  pair->next = last->next;
  last->next = pair;
}

/**
 * {@inheritdoc}
 */
TAB_CALL void tab_del (struct tab *tab, char *name)
{
  assert(tab != 0);
  assert(name != 0);

  struct tab_ent *item;
  struct tab_par *pair;
  struct tab_par *last = 0;

  u32 hash = hash_gen(name);
  u32 offs = hash & tab->mask;

  item = tab->data[offs];
  if (!item) {
    return;
  }

  pair = item->pair;
  while (pair) {
    if (strcmp(pair->name, name) == 0) {
      if (last) {
        last->next = pair->next;
      } else {
        item->pair = pair->next;
      }
      free_par(pair);
      tab->elms--;
      break;
    }
    last = pair;
    pair = pair->next;
  }
} 

/**
 * {@inheritdoc}
 */
TAB_CALL void tab_free (struct tab *tab) 
{
  assert(tab != 0);

  for (u32 i = 0, l = tab->size; i < l; ++i) {
    if (tab->data[i]) {
      free_ent(tab->data[i]);
    }
    mem_free(tab->data[i]);
  }

  mem_free(tab->data);
  tab->size = 0;
  tab->elms = 0;
  tab->mask = 0;
  tab->data = 0;
}

/* --------------------------------------------------- */

/** grows the table */
static void grow_mem (struct tab *tab, szt size)
{
  tab->size = size;
  tab->mask = size - 1;
  size *= sizeof(struct tab_ent *);
  tab->data = mem_grow(tab->data, size);
}

/**
 * DJBX33A (Daniel J. Bernstein, Times 33 with Addition)
 * Ralf S. Engelschall <rse@engelschall.com>
 */
static u32 hash_gen (char *s)
{
  u32 l = strlen(s);
  u32 h = 5381;

  for (; l >= 8; l -= 8) {
    h = ((h << 5) + h) + *s++;
    h = ((h << 5) + h) + *s++;
    h = ((h << 5) + h) + *s++;
    h = ((h << 5) + h) + *s++;
    h = ((h << 5) + h) + *s++;
    h = ((h << 5) + h) + *s++;
    h = ((h << 5) + h) + *s++;
    h = ((h << 5) + h) + *s++;
  }

  switch (l) {
    case 7: h = ((h << 5) + h) + *s++;
    case 6: h = ((h << 5) + h) + *s++;
    case 5: h = ((h << 5) + h) + *s++;
    case 4: h = ((h << 5) + h) + *s++;
    case 3: h = ((h << 5) + h) + *s++;
    case 2: h = ((h << 5) + h) + *s++;
    case 1: h = ((h << 5) + h) + *s++;
  }

  return h | 0x80000000;
}

/** re-hashes the table */
static void hash_tab (struct tab *tab)
{
  u32 size = tab->size;
  size *= sizeof(struct tab_ent *);

  struct tab new = *tab;
  struct tab_ent *item;
  struct tab_ent *nxti;
  struct tab_par *pair;
  struct tab_par *nxtp;

  new.head = 0;
  new.data = mem_aodt(size);

  item = tab->head;
  while (item) {
    pair = item->pair;
    while (pair) {
      nxtp = pair->next;
      pair->next = 0;
      put_pair(&new, pair);
      pair = nxtp;
    }
    nxti = item->next;
    mem_free(item);
    item = nxti;
  }

  mem_free(tab->data);
  tab->data = new.data;
  tab->head = new.head;
}

/** inserts a pair into the table */
static void put_pair (struct tab *tab, struct tab_par *pair)
{
  struct tab_ent *item;
  u32 offs = pair->hash & tab->mask;
  
  item = tab->data[offs];
  if (!item) {
    item = make_ent();
    item->next = tab->head;
    item->pair = pair;
    tab->head = item;
    tab->data[offs] = item;
    return;
  }

  pair->next = item->pair;
  item->pair = pair;
}

/** creates a new entry */
static struct tab_ent * make_ent(void)
{
  return mem_aodt(sizeof(struct tab_ent));
}

static struct tab_par * make_par(u32 hash)
{
  struct tab_par *pair;
  pair = mem_aodt(sizeof(struct tab_par));
  pair->hash = hash;
  return pair;
}

/** creates a new pair */
static void free_par (struct tab_par *pair)
{
  mem_free(pair->name);
  mem_free(pair->data);
  mem_free(pair);
}

/** releases a entry */
static void free_ent (struct tab_ent *item)
{
  struct tab_par *pair;
  struct tab_par *next;

  pair = item->pair;
  while (pair) {
    next = pair->next;
    free_par(pair);
    pair = next;
  }

  free(item);
}

