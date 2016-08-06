/* 
 * File: sjf.c
 * Author: Alex Brodsky
 * Purpose:   This is an implementation of the Shortest Job First Scheduler.
 */

#include <stdio.h>
#include <assert.h>
#include <pthread.h>

#include "rcb.h"
#include "vss.h"

static void init();
static void submit( rcb * r );
static rcb * get_next( void );

vss sjf_scheduler = { "SJF", &init, &submit, &get_next };

static rcb *head; /* head of ready queue */
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/* This function initializes the scheduler
 * Parameters: None
 * Returns: None
 */
static void init() {
}

/* This function checks if there are any web clients waiting to connect.
 *    If one or more clients are waiting to connect, this function returns.
 *    Otherwise, this function puts the program to sleep (blocks) until
 *    a client connects.
 * Parameters: None
 * Returns: None
 */
static void submit( rcb * r ) {
  rcb *tmp;

  assert( r );                               /* sanity check */
                                    
  /* insert rcb into priority queue, based on # of bytes left to send.
   */
  pthread_mutex_lock( &lock );
  if( !head || ( r->left < head->left ) ) {  /* before head */      
    r->next = head;
    head = r;
  } else {                                   /* after head */
    /* walk the list until the next rcb contains a lower priority request */
    for(tmp = head; tmp->next && (tmp->next->left <= r->left); tmp = tmp->next);

    /* insert RCB at this point */
    r->next = tmp->next;
    tmp->next = r;
  }
  pthread_mutex_unlock( &lock );
}


/* This function checks if there are any web clients waiting to connect.
 *    If one or more clients are waiting to connect, this function opens
 *    a connection to the next client waiting to connect, and returns an
 *    integer file descriptor for the connection.  If no clients are 
 *    waiting, this function returns -1.
 * Parameters: None
 * Returns: A positive integer file decriptor to the next clients connection,
 *          or -1 if no client is waiting.
 */
static rcb * get_next( void ) {
  rcb *r;

  pthread_mutex_lock( &lock );
  r = head;                      /* remove first item from the queue */
  if( r ) {                      /* if queue is not empty */
    head = head->next;           /* unlink head */
    r->next = NULL;
  }
  pthread_mutex_unlock( &lock );
  return r;
}
