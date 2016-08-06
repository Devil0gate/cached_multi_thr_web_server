/* 
 * File: sws.c
 * Author: Alex Brodsky
 * Purpose: This file contains the implementation of a simple web server.
 *          It consists of two functions: main() which contains the main 
 *          loop accept client connections, and serve_client(), which
 *          processes each client request.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdarg.h>

#include "network.h"
#include "scheduler.h"
#include "cache_api.h"
#include "queue.h"
#include "serve.h"
#include "rcb.h"

static queue work;                         /* work queue */

void safe_printf( char * fmt, ... ) {
  static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  va_list ap;
  va_start( ap, fmt );

  if( pthread_mutex_lock( &lock ) ) abort();
  vprintf( fmt, ap );
  fflush( stdout );
  pthread_mutex_unlock( &lock );
}

static void *worker( void * arg ) {
  rcb *req;
  int block = 0;
  int valid;  /* cached file descriptor */
  for( ;; ) {
    req = queue_dequeue( &work, block );
    if( req ) {
      if( serve_start( req ,&valid) ) {                /* process each client */
        scheduler_submit( req );
        safe_printf( "Request for file %s admitted.\n", req->path );
      } else {
        close( req->client );
        rcb_free( req );
      }
    } else {
      req = scheduler_get_next();
      printf("the returned cfd is: %d\n\n", valid);
      if( req && serve_continue( req, &valid ) ) {
        safe_printf( "Sent %d bytes of file %s.\n", req->last, req->path );
        scheduler_submit( req );
      } else if( req ) {
        safe_printf( "Sent %d bytes of file %s.\n", req->last, req->path );
        fclose( req->file );
        close( req->client );
        safe_printf( "Request for file %s completed.\n", req->path );
        fflush( stdout );
        rcb_free( req );
        /* close from cache */
      }
    }
    block = req == NULL;
  }
  return NULL;
}


/* This function is where the program starts running.
 *    The function first parses its command line parameters to determine port #
 *    Then, it initializes, the network and enters the main loop.
 *    The main loop waits for a client (1 or more to connect, and then processes
 *    all clients by calling the seve_client() function for each one.
 * Parameters: 
 *             argc : number of command line parameters (including program name
 *             argv : array of pointers to command line parameters
 * Returns: an integer status code, 0 for success, something else for error.
 */
int main( int argc, char **argv ) {
  int port = -1;                                   /* server port # */
  int num;                                         /* number of threads */
  int fd;                                          /* client file descriptor */
  rcb *req;                                        /* next request to process */
  int size;
  int i;
  pthread_t tid;

  /* check for and process parameters 
   */
  if( ( argc < 5 ) || ( sscanf( argv[1], "%d", &port ) < 1 ) 
                   || ( sscanf( argv[3], "%d", &num ) < 1 ) 
                   || (sscanf( argv[4], "%d", &size)) < 1) {
    printf( "usage: sms <port> <scheduler> <threads>\n" );
//    return 0;
  } 

  cache_size = size;

  scheduler_init( argv[2] );                        /* init scheduler */
  network_init( port );                             /* init network module */
  queue_init( &work );                              /* init work queue */
  cache_init(cache_size);
  fcb_init();
  rcb_init();
  free_list_init(&f_list);

  for( i = 0; i < num; i++ ) {
    if( pthread_create( &tid, NULL, &worker, NULL ) ) {
      printf( "pthread_create() failed\n" );
      return 0;
    }
  }

  for( ;; ) {                                       /* main loop */
    network_wait();                                 /* wait for clients */
    
    for( fd = network_open(); fd >= 0; fd = network_open() ) { /*get clients*/
      req = rcb_alloc();
      req->client = fd;                             /* init RCB */
      queue_enqueue( &work, req );
    }
  }
}
