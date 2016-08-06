/* 
 * File: queue.c
 * Author: Alex Brodsky
 * Purpose:  This implements a general RCB queue with a linked list 
 */

#include <stdio.h>
#include <assert.h>

#include "free_list.h"

static pthread_mutex_t mutex_init = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_init = PTHREAD_COND_INITIALIZER;

/* This function takes a pointer to a queue and initializes the queue.
 * Parameters: q: pointer to queue
 * Returns: void
 */
extern void free_list_init( free_list* q ) {
  assert( q );
  int i;
  q->lock = mutex_init;
  q->not_empty = cond_init;
  free_init();
  for(i = 0;i<100;i++){
    free_blk* cur;
    cur = free_alloc();
    free_enq(q, cur);
    cur->c_index = i;
  }

}

/* This function takes a pointer to a queue and a pointer to an RCB
 *    and enqueues the RCB onto the queue.
 * Parameters: q : pointer to a queue
               r : pointer to a RCB
 * Returns: None
 */
extern void free_enq( free_list* q, free_blk* r ) {
  assert( q );
  assert( r );

  r->next = NULL;
  pthread_mutex_lock( &q->lock );
  if( !q->head ) {         /* if emptty */
    q->head = r;           /* set head */
  } else {                 /* else */
    q->tail->next = r;     /* add to tail */
  }
  q->tail = r;             /* set tail */
  pthread_cond_signal( &q->not_empty );
  pthread_mutex_unlock( &q->lock );
}

/* This function takes a pointer to a queue and, and removes and returns 
 *    the first RCB in the queue.  If queue is empty and wait is false, 
 *    it returns NULL, and otherwise block until queue is not empty.
 * Parameters: q: pointer to queue
 *             wait: if true, block if empty
 * Returns: Pointer to RCB removed from the queue.
 */
extern free_blk* free_deq( free_list* q ) {
  free_blk* r;

  pthread_mutex_lock( &q->lock );
  if( !q->head ) {
    pthread_cond_wait( &q->not_empty, &q->lock );
  }

  r = q->head;
  if( q->head ) {             /* if not empty */
    q->head = q->head->next;  /* remove first elemenet */
    r->next = NULL;
  }
  pthread_mutex_unlock( &q->lock );
  return r;
}
