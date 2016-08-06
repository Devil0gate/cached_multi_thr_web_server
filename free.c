/* 
 * File: rcb.c
 * Author: Alex Brodsky
 * Purpose: This file contains RCB manager, including initialization, alloc
 *          and free.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>

#include "free.h"


static free_blk free_list[100];             /* request table */
static free_blk *freelist;

static pthread_mutex_t free_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t free_avail = PTHREAD_COND_INITIALIZER;

void free_init() {
  int i;

  freelist = free_list;                             /* create RCB free list */
  for( i = 0; i <  99; i++ ) {
    free_list[i].next = &free_list[i+1];
  }
}


free_blk* free_alloc( void ) {
  free_blk* r;

  pthread_mutex_lock( &free_lock );
  if( !freelist ) {
    pthread_cond_wait( &free_avail, &free_lock );
  }
  assert( freelist );
  r = freelist;                                 /* allocate RCB */
  freelist = freelist->next;
  pthread_mutex_unlock( &free_lock );
  memset( r, 0, sizeof( free_blk ));
  return r;
}


void free_free( free_blk *r ) {
  assert( r );

  pthread_mutex_lock( &free_lock );
  r->next = freelist;
  freelist = r;
  pthread_cond_signal( &free_avail );
  pthread_mutex_unlock( &free_lock );
}

