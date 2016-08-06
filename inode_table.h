/*
* File: inode_table.h
* Author: Daniel
* Purpose: this file contains the prototype for a simple file entry table and a file descriptor table /all (list) structure
**/

#ifndef INODE_TABLE_H
#define INODE_TABLE_H

#include <stdio.h>
#include "fcb.h"
#include "pthread.h"

#define MAX_PAGE 100
#define MAX_FILE 100

/*
* 
*
*
**/

int count;

typedef struct _entry_table {
  int full;
  fcb* head;
  fcb* tail;
  pthread_mutex_t lock;
  pthread_cond_t not_empty;
} entry_table;

extern void cache_insert(entry_table* entry, fcb* f);

extern fcb* cache_removal(entry_table* entry, fcb* f); /* wait might not be necessary */

#endif

