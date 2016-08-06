/*
 * File: fcb.h
 * Author: Daniel
 * Purpose: manage the allocation for files through file control block (a similar look up data structure 
 * for file directory)
*/

#ifndef FREE_H
#define FREE_H

#include <stdio.h>
#include "pthread.h"

#define LIMIT_LENTH_PER 4096


typedef struct _free free_blk;

struct _free {
  int c_index;
  free_blk* next;
};

void free_init();

free_blk* free_alloc();

void free_free(free_blk* r);

#endif

