/* 
 * File: serve.h
 * Author: Alex Brodsky
 * Purpose: This file contains the prototypes for routines that serve the 
 *          client. There are two functions: serve_start(), which processes
 *          the client request, and serve_continue(),  which serves
 *          a portion of the client's request
 */

#ifndef SERVE_H
#define SERVE_H

#include "rcb.h"

/* This function takes an RCB of a client, reads in the request, 
 *    parses the request, and sets up the requested file.  If the
 *    request is improper or the file is not available, the appropriate
 *    error is sent back.
 * Parameters: 
 *             r : rcb of prospective client
 * Returns: 1 if reuqest is proper, 0 otherwise.
 */
extern int serve_start( rcb *r, int* v );


/* This function takes an rcb of a client and sends back a portion of
 *    the client's request
 * Parameters: 
 *             req : RCB for client
 * Returns: 1 if request is not complete, 0 if request is completed
 */
extern int serve_continue( rcb *req, int* v );

#endif
