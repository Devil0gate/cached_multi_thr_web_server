/*
*
*
**/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "inode_table.h"
#include "fcb.h"

static pthread_mutex_t mutex_init = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_init = PTHREAD_COND_INITIALIZER;

/* 
static int frame_full( entry_table* entry){
  return entry->full;
}


**/
extern void table_init( entry_table* entry){
  assert( entry);

  entry->lock = mutex_init;
  entry->not_empty = cond_init;
}

/* create a new file entry */
extern void cache_insert( entry_table* entry, fcb* f){
  assert( entry);
  assert( f);

  f->next = NULL;
  pthread_mutex_lock( &entry->lock);

  if( !entry->head){
    entry->head = f;
  } else {
    entry->tail->next = f;
  }
  entry->tail = f;
  count++;
  pthread_cond_signal( &entry->not_empty);
  pthread_mutex_unlock( &entry->lock);
}

/* remove an entry ( LRU replacement algorithm )*/
extern fcb* cache_removal( entry_table* entry, fcb* f){


  pthread_mutex_lock( &entry->lock);
  if(!entry->head) {
    pthread_cond_wait( &entry->not_empty, &entry->lock);
  }
/*
  f = entry->head;
  if( entry->head){
    entry->head = entry->head->next;
    f->next = NULL;
  }
*/
  fcb* head = entry->head;
  assert(head);
  if(head == f){
    head->next = head->next->next;
   
  }else{
    fcb* prev = head;
    while(prev->next != NULL && prev->next != f)
      prev = prev->next;
      assert(prev->next);
    prev->next = prev->next->next;
  }
  count--;
  pthread_mutex_unlock( &entry->lock );
  return f;
}

/* find specific fcb */
extern int cache_find(entry_table* entry, fcb* cur){
  int index = 0;
  fcb* head = entry->head;
  if(cur == entry->head){
    return index;
  } else{
    while(head){
      if(cur == head)
        return index;
      head = head->next;
    }
  }
  return -1;
}


