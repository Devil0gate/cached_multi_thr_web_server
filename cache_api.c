
/*
* File: cache.h
* Author: Daniel
* Purpose: Defines file system API for cache algorithm 
**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include "fcb.h"
#include "free_list.h"
#include "cache_api.h"
#include "fd_table.h"
#include "inode_table.h"


static file_dt fd_table[100];
static entry_table cache_swoft; //swoft
//static free_list f_list;

/*--------- initialize procedure not done yet ------------*/
void cache_init( int size ){
  global_cache = malloc(size);

  if(!global_cache) 
    perror("malloc_failed!");
}

/*------------ least recently used ------------------*/
static void least_recently_used(int limit){
  fcb* cur;
  fcb* temp;
  e_queue* equeue;
  extend* extend;
  free_blk* free;
  int sum;
  assert(cache_swoft.head);
    cur = cache_swoft.head;
  do{
    if(cur->refern_num == 0){
      cur = cache_removal(&cache_swoft, cur); // change back
      equeue = cur->equeue;
      extend = equeue->head;
      /* array needs to be initialized */
      while(equeue){
        extend = e_deq(equeue);
//        free = linear_search_free(extend->ptr);
        free = free_alloc();
        free_enq(&f_list, free);
      }
    }
    temp = cur;
    cur = cur->next;
    sum += cur->limit;
    fcb_free(temp);
  } while(limit < sum);
} 

/*---------------linearly go to index! swoft table---------------------*/
static fcb* linear_search_swoft(int index){
  int i;
  fcb* cur;
  cur = cache_swoft.head;
  assert(cur);
  /* careful with off-by-one */
  for( i = 0; i < index; i++){
    cur = cur->next;
    i++;
  }
  return cur;
}

/* If the file is not in the cache, load it into the cache. Return a “cached file descriptor”
* (CFD), which refers to the file in the cache. Note, each worker thread should get its
* own CFD when opening a file. This function returns −1 if the file cannot be accessed.
* If a thread requests a file that is being loaded into the cache, the thread should block
* until the file is loaded. (not done yet)
* If the file cannot be loaded because the file is too big, a CFD should still be returned.
* The file should be opened and the cache should read the file from disk when it is needed,
* instead of storing it in memory. */
int cache_open( char* file){
  fcb* f_b;
  int left_read = 0;
  int i;
  int j;
  int len;
  int index;  /* index in open table positioned at found file */
  int found = 0;
  int inode;
  int fd;
  int offset;
  int num_free = 0;
  free_blk* free;
  free_blk* temp;
  extend* ex;
  e_queue* equeue = malloc(sizeof(e_queue));
  struct stat file_struct;

  /* the file is found in swoft table, dont have to write anything on cache */
  assert(file);
    if(stat(file, &file_struct)){
      printf("Bad call\n");
      abort();
    }
    inode = file_struct.st_ino;
    if(cache_swoft.head){
      if(cache_swoft.head->i_node == inode){
      /* found */
      found = 1;
      index = 0;
      }
      else{
        f_b = cache_swoft.head;
        do{
          index++;
          f_b = f_b->next;
          if(!f_b){
            printf(" the inode cannot be found! ");break;}
        } while(f_b->i_node != inode);
        if(f_b){
          if(f_b->i_node == inode)
            found = 1;
        }
      }
    }
      
    /* exclusively for client (allocate the new fd table) */
    int cfd;
    cfd = fd_table[fd_table->size].cfd + 1;
    fd_table->size++;
    fd_table[cfd].cfd = cfd;

    if(!found){
      fd = open(file, O_RDONLY);
      if( fd == -1)
	perror("open");
      if(cache_size > file_struct.st_size) {
        f_b = fcb_alloc();
        f_b->i_node = inode;
        f_b->f_size = file_struct.st_size;
        f_b->limit = file_struct.st_size/4096;
        f_b->equeue = equeue;
        j = file_struct.st_size/4096; /* */
      
        i = 0;/* limit is how many blocks that I need for current file */
          int left = file_struct.st_size%4096;          
          if(left > 0 && j>=1){
            left_read = 1;
            j++;
          }
          temp = f_list.head;
          assert(temp);
          while(temp){
            temp = temp->next;num_free++;
            printf("fuck!%d\n\n", num_free);
          }
          
          if(j > num_free)
            least_recently_used(j-num_free);
          free = f_list.head;

          do{
              offset = free->c_index*4096;
              extend* e = malloc(sizeof(extend));
              e_enq(equeue, e);
              equeue->tail->ptr = free->c_index;
              //printf("the index is: %d\n\n", free->c_index);
              if(left_read){
                len = read(fd, &global_cache[offset], left);left_read = 0;                
              } else{
                len = read(fd, &global_cache[offset], 4096);
              }
              e->size = len;    
            i++;
            temp = free;
            free = free->next;
            free_deq(&f_list);
            free_free(temp);
          } while(i < j);
          ex = equeue->head;
          while(ex){
            printf("the assigned index is %d\n\n", ex->ptr);
            ex = ex->next;
          }
          fd_table[cfd].seek_cur = f_b->equeue->head->ptr*4096;
          close(fd);
          /* when finishing loading, create a new entry to swoft table */
          
          cache_insert(&cache_swoft, f_b);
          fd_table[cfd].inx = count;
          fd_table[cfd].left = file_struct.st_size;
      } else{
        cfd = -1;
      }
    } else {
      /* found file in open wide table */
      fd_table[cfd].inx = index;
    }
    printf("returned cfd is %d\n\n", cfd);
    return cfd;
}

/*  The next n bytes of the file referred to by the cfd should be sent to the client by
* performing a write with the (client) file descriptor. The value n is the maximum number
* of bytes to send. If the number of bytes remaining is r < n, then r bytes should be sent.
* The function returns the number of bytes sent.
* If the file referred to by the cfd is too big to fit into the cache, it should be read from
* disk instead of from the cache.
fd**/
int cache_send( int fd, int client, int n){
  /* the mechanism to check whether the 
  *  file can be sent through cache or it
  *  has to be checked through disk
  */
  int len;
  int sent;
  int left;
  int f_index;
  int offset;
  int not_done = 0;
  e_queue* equeue;
  extend* exd;
  free_blk* free;
  fcb* cur;
  printf("the cfd is %d\n\n", fd);
  f_index = fd_table[fd].inx;  /* index for swoft table */
  cur = linear_search_swoft(f_index);
  assert(cur);
  equeue = cur->equeue;
  offset = fd_table[fd].seek_cur;
  left = fd_table[fd].left; /* are u done yet? */
  printf("the real bytes are, %d\n\n", left);
  exd = equeue->head;

  do {
    if(n - sent < exd->size){
      len = write(fd, &global_cache[offset], n - sent);
      sent += len;
      exd->size-= n - sent;
      offset = exd->ptr*4096 + (n - sent);
      break;
    }
    len = write(fd, &global_cache[offset], exd->size);
    sent +=len;
    
    free = free_alloc();
    free_enq(&f_list, free);
    free->c_index = exd->ptr;
    exd = exd->next;
    e_deq(equeue);
    if(!exd)
      break;
    offset = exd->ptr*4096;
    } while(1);
    printf("cache sent bytes is %d and we should send back %d\n\n", sent, n);    
    printf("the left bytes of an file is: %d\n\n", left);
      fd_table[fd].seek_cur = offset;
      fd_table[fd].left = left - n;
      /* gurantee atomic */
      if(cache_swoft.head->next){
        cache_removal(&cache_swoft, cur);
        /* LRU */
        cache_insert(&cache_swoft, cur);
      }
        fd_table[fd].inx = count - 1;
      if(fd_table[fd].left > 0)
        not_done = 1;
  return not_done;
}

int cache_filesize( int fd){
  int index;
  fcb* cur;
  index = fd_table[fd].inx;
  cur = linear_search_swoft(index);
  
  return cur->f_size;    
}

/* what exactly does it return */
int cache_close( int fd){
  fcb* f;
  /* if fd is negtive, close the file */
  f = linear_search_swoft(fd_table[fd].inx);
  f->refern_num--;
  return 0;
}


