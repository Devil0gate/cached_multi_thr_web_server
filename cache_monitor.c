/*-------cache monitor----------*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include "cache_monitor.h"
#include "pthread.h"

static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mwrite = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mread = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t rallow = PTHREAD_MUTEX_INITIALIZER;

static int rc = 0;
static int wc = 0;



extern int cache_writer(char* file){
  int fd;
  pthread_mutex_lock(&mwrite);
  wc++;
  if(wc==1)
    pthread_mutex_lock(&rallow);
    pthread_mutex_unlock(&mwrite);
    pthread_mutex_lock(&mutex1);
      /* cache send */
      fd = cache_open(file);
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_lock(&mwrite);
    wc--;
    if(wc==0)
      pthread_mutex_unlock(&rallow);
      pthread_mutex_unlock(&mwrite);
      // pthread_exit(0);
    return fd;
}

extern int cache_reader(int fd, int client, int n){
  int i;
  pthread_mutex_lock(&rallow);
  pthread_mutex_lock(&mread);
  rc++;
  if(rc==1)
    pthread_mutex_lock(&mutex1);
    pthread_mutex_unlock(&mread);
    pthread_mutex_unlock(&rallow);
      /* cache open */
     i = cache_send(fd, client, n);
    pthread_mutex_lock(&mread);
    rc--;
    if(rc==0)
      pthread_mutex_unlock(&mutex1);
      pthread_mutex_unlock(&mread);
      //pthread_exit(0);
    return i;
}

