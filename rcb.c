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

#include "rcb.h"

#define MAX_REQS 100                       /* maximum number of requests */

static rcb requests[MAX_REQS];             /* request table */
static rcb *free_rcb;

static pthread_mutex_t rcb_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t rcb_avail = PTHREAD_COND_INITIALIZER;

void rcb_init() {
  int i;

  free_rcb = requests;                             /* create RCB free list */
  for( i = 0; i < MAX_REQS - 1; i++ ) {
    requests[i].next = &requests[i+1];
  }
}


rcb *rcb_alloc( void ) {
  rcb *r;

  pthread_mutex_lock( &rcb_lock );
  if( !free_rcb ) {
    pthread_cond_wait( &rcb_avail, &rcb_lock );
  }
  assert( free_rcb );
  r = free_rcb;                                 /* allocate RCB */
  free_rcb = free_rcb->next;
  pthread_mutex_unlock( &rcb_lock );
  memset( r, 0, sizeof( rcb ) );
  return r;
}


void rcb_free( rcb *r ) {
  assert( r );

  pthread_mutex_lock( &rcb_lock );
  r->next = free_rcb;
  free_rcb = r;
  pthread_cond_signal( &rcb_avail );
  pthread_mutex_unlock( &rcb_lock );
}
