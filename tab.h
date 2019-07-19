#pragma once

#include "def.h"

struct tab {
  u32 size;
  u32 elms;
  u32 mask;
  struct tab_ent *head;
  struct tab_ent **data;
};

struct tab_ent {
  struct tab_par *pair;
  struct tab_ent *next;
};

struct tab_par {
  u32 hash;
  char *name;
  void *data;
  struct tab_par *next;
};

/**
 * initializes a new table
 * 
 * @param the table to initialize
 * @return the initialized table
 */
extern void tab_init (struct tab *);

/**
 * retrieves a item in the table
 * 
 * @param the table
 * @param the name of the item
 * @return the data associated with the given name or NULL if not found
 */
extern void * tab_get (struct tab *, char *);

/**
 * adds a item to the table
 * 
 * @param the table
 * @param the name of the item
 * @param the data to store
 * @param the size of the data
 */
extern void tab_put (struct tab *, char *, void *, szt);

/**
 * deletes a item in the table
 * 
 * @param the table
 * @param the name of the entry
 */
extern void tab_del (struct tab *, char *);

/**
 * releases all entries in the table
 * 
 * @param the table
 */
extern void tab_free (struct tab *);
