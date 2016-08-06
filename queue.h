/* 
 * File: queue.h
 * Author: Alex Brodsky
 * Purpose: This file contains the prototypes for a simple queue data structure.
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "rcb.h"
#include "pthread.h"

/* 
 * This module has three functions:
 *   queue_init() : initializes the queue.
 *   queue_enqueue() : enqueues the RCB into the queue.
 *   queue_dequeue() : returns next RCB in the queue or NULL if empty.
 *
 * The queue_enqueue() function takes a pointer to an RCB and a queue, and 
 * and enqueues.  The RCB should not be in any other queue.
 *
 * The queue_dequeue() function takes a pointer to a queue and a wait
 * flag.  It returns the next item in the queue if the queue is not empty.
 * Otherwise, if flag is true, it blocks until an item is available, or
 * it returns NULL.
 */

typedef struct _queue {
  rcb * head;              /* queue head */
  rcb * tail;              /* queue tail */
  pthread_mutex_t lock;     /* mutex lock to make thread safe */
  pthread_cond_t not_empty; /* mutex lock to make thread safe */
} queue;

/* This function takes a pointer to a queue and initializes the queue.
 * Parameters: q: pointer to queue
 * Returns: void
 */
extern void queue_init( queue * q );

/* This function takes a pointer to a queue and a pointer to an RCB
 *    and enqueues the RCB onto the queue.
 * Parameters: q : pointer to a queue
               r : pointer to a RCB
 * Returns: None
 */
extern void queue_enqueue( queue * q, rcb * r );


/* This function takes a pointer to a queue and, and removes and returns 
 *    the first RCB in the queue.  It returns NULL if the queue is empty.
 * Parameters: q: pointer to queue
               wait: if true, then block until queue is not empty
 * Returns: Pointer to RCB removed from the queue.
 */
extern rcb * queue_dequeue( queue * q, int wait );

#endif
