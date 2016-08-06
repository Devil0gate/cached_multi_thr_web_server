#include <assert.h>
#include <pthread.h>

#include <stdlib.h>
#include <string.h>

#include "fcb.h"

static fcb file_block[100];
static fcb* free_block;

static pthread_mutex_t fcb_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t fcb_avail = PTHREAD_COND_INITIALIZER;

void fcb_init(){
  int i;

  free_block = file_block;
  /* keep the refernce here */
  for( i = 0; i < 100; i++){
    file_block[i].next = &file_block[i+1];
  }
}

fcb* fcb_alloc(void ){
  fcb* f;

  pthread_mutex_lock( &fcb_lock);
  if( !free_block){
    pthread_cond_wait( &fcb_avail, &fcb_lock);
  }
  assert( free_block);

  f = free_block;
  free_block = free_block->next;
  pthread_mutex_unlock( &fcb_lock);
  memset(f, 0, sizeof( fcb));
  return f;
}

void fcb_free(fcb* f ){
  assert(f);

  pthread_mutex_lock( &fcb_lock);
  f->next = free_block;
  free_block = f;
  pthread_cond_signal( &fcb_avail );
  pthread_mutex_unlock( &fcb_lock);
}


