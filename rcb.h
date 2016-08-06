/* 
 * File: rcb.h
 * Author: Alex Brodsky
 * Purpose: Defines the Request Control Block for managing requests.
 */

#ifndef RCB_H
#define RCB_H

#include <stdio.h>

#define MAX_HTTP_SIZE 8192                 /* size of buffer to allocate */

typedef struct _rcb rcb;
struct _rcb {
  rcb *     next;    /* pointer to next rcb in queue */
  int       client;  /* client file descriptor */
  FILE *    file;    /* FILE stream of requested file */
  int       left;    /* number of bytes left to send */
  int       max;     /* maximum allowed send */
  int       last;    /* last amount of bytes sent */
  char      path[FILENAME_MAX];         /* path of requested file */
  char      buffer[MAX_HTTP_SIZE];  /* communication buffer */
};

extern void rcb_init();
extern rcb *rcb_alloc( void );
extern void rcb_free( rcb *r );

#endif


