/*
 * File: fcb.h
 * Author: Daniel
 * Purpose: manage the allocation for files through file control block (a similar look up data structure 
 * for file directory)
*/

#ifndef FCB_H
#define FCB_H

#include <stdio.h>
#include "e_queue.h"
#include "pthread.h"

#define LIMIT_LENTH_PER 4096


typedef struct _fcb fcb;

struct _fcb {
  int limit; /* number of blocks needed for writing current file */
  int refern_num;/* keep tracking open file */
  int i_node;
  int f_size;
  e_queue* equeue;
  fcb* next;
};

void fcb_init();

fcb* fcb_alloc();

void fcb_free(fcb* f);

#endif
