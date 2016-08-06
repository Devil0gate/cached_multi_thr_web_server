/* 
 * File: queue.c
 * Author: Alex Brodsky
 * Purpose:  This implements a general RCB queue with a linked list 
 */

#include <stdio.h>
#include <assert.h>

#include "fcb.h"
#include "e_queue.h"


/* This function takes a pointer to a queue and a pointer to an RCB
 *    and enqueues the RCB onto the queue.
 * Parameters: q : pointer to a queue
               r : pointer to a RCB
 * Returns: None
 */
extern void e_enq( e_queue* eq, extend* e ) {
  assert( eq );
  assert( e );

  e->next = NULL;
  if( !eq->head ) {         /* if emptty */
    eq->head = e;           /* set head */
  } else {                 /* else */
    eq->tail->next = e;     /* add to tail */
  }
  eq->tail = e;             /* set tail */
}

/* This function takes a pointer to a queue and, and removes and returns 
 *    the first RCB in the queue.  If queue is empty and wait is false, 
 *    it returns NULL, and otherwise block until queue is not empty.
 * Parameters: q: pointer to queue
 *             wait: if true, block if empty
 * Returns: Pointer to RCB removed from the queue.
 */
extern extend* e_deq( e_queue* eq ) {
  extend* e;

  e = eq->head;
  if( eq->head ) {             /* if not empty */
    eq->head = eq->head->next;  /* remove first elemenet */
    e->next = NULL;
  }
  return e;
}


