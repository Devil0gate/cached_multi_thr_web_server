/* 
 * File: serve.c
 * Author: Alex Brodsky
 * Purpose: This file contains the routines which serve the client
 *          It consists of two functions: serve_start(), which processes
 *          the client request, and serve_continue(),  which serves
 *          a portion of the client's request
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>

#include "rcb.h"
#include "serve.h"
#include "cache_monitor.h"
/* This function takes an RCB of a client, reads in the request, 
 *    parses the request, and sets up the requested file.  If the
 *    request is improper or the file is not available, the appropriate
 *    error is sent back.
 * Parameters: 
 *             r : rcb of prospective client
 * Returns: 1 if reuqest is proper, 0 otherwise.
 */
int serve_start( rcb *r, int* valid ) {
  struct stat st;                                   /* struct for file size */
  char *req = NULL;                                 /* ptr to req file */
  char *brk;                                        /* state used by strtok */
  char *tmp;                                        /* error checking ptr */
  FILE *fin;                                        /* input file handle */
  int len = 0;                                      /* length of data read */
  int left = MAX_HTTP_SIZE;                         /* amount of buffer left */
  int* fd = malloc(sizeof(int));

  for( tmp = r->buffer; !strchr( tmp, '\n' ); left -= len ) { /* read req line*/
    tmp += len;
    len = read( r->client, tmp, left );             /* read req from client */
    if( len <= 0 ) {                                /* if read incomplete */
      perror( "Error while reading request" );      /* no need to go on */
      return 0;
    }
  } 

  /* standard requests are of the form
   *   GET /foo/bar/qux.html HTTP/1.1
   * We want the second token (the file path).
   */
  tmp = strtok_r( r->buffer, " ", &brk );            /* parse request */
  if( tmp && !strcmp( "GET", tmp ) ) {
    req = strtok_r( NULL, " ", &brk );
  }
 
  if( !req ) {                                      /* is req valid? */
    len = sprintf( r->buffer, "HTTP/1.1 400 Bad request\n\n" );
    write( r->client, r->buffer, len );             /* if not, send err */
  } else {                                          /* if so, open file */
    req++;                                          /* skip leading / */
    strncpy( r->path, req, FILENAME_MAX );
      fin = fopen( r->path, "r" );                     /* open file in the cache*/
      if( !fin ) {                                    /* check if successful */
        len = sprintf( r->buffer, "HTTP/1.1 404 File not found\n\n" );  
        write( r->client, r->buffer, len );           /* if not, send err */
      } else if( !fstat( fileno( fin ), &st ) ) {     /* if so, start request */
      len = sprintf( r->buffer, "HTTP/1.1 200 OK\n\n" );/* send success code */
      write( r->client, r->buffer, len );

      r->file = fin;
      r->left = st.st_size;
      } else {
        fclose( fin );
      }
      *fd = cache_writer(r->path);  
      valid = fd;        
  }
  return r->file != NULL;                          /* if file set, req valid */
}


/* This function takes an rcb of a client and sends back a portion of
 *    the client's request
 * Parameters: 
 *             req : RCB for client
 * Returns: 1 if request is not complete, 0 if request is completed
 */
int serve_continue( rcb *req, int* valid ) {
  int len;                                          /* length of data read */
  int n;                                            /* amount to send */  
  int fd;
  fd = *valid;
  n = req->left;                                     /* compute send amount */
  if( !n ) {                                         /* if 0, we're done */
    return 0;
  } else if( req->max && ( req->max < n ) ) {        /* if there is limit */
    n = req->max;                                    /* send upto the limit */
  }
  req->last = n;                                    /* remember send size */
  /* read from the disk<----- write to client -----> */
  if(fd == -1){
    do {                                              /* loop, read & send file */
      len = n < MAX_HTTP_SIZE ? n : MAX_HTTP_SIZE;    /* how much to read */
      len = fread( req->buffer, 1, len, req->file );  /* read file chunk */
      if( len < 1 ) {                                 /* check for errors */
        perror( "Error while reading file" );
        return 0;
      } else if( len > 0 ) {                          /* if none, send chunk */
        len = write( req->client, req->buffer, len );
        if( len < 1 ) {                               /* check for errors */
          perror( "Error while writing to client" );
          return 0;
          }
          req->left -= len;                              /* reduce what remains */
          n -= len;
        } 
    } while( ( n > 0 ) && ( len == MAX_HTTP_SIZE ) );  /* the last chunk < 8192 */
      return req->left > 0;

  }else {
  /* writing to disk from cache_api (reader) */
     return cache_reader(fd, req->client, n);         /* return true if not done */
  }                                                 
}

