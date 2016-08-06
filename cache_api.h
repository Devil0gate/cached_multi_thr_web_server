/*
* File: cache_api.h
* Author: Daniel
* Purpose: Define the cache file system API
**/

#ifndef CACHE_API_H
#define CACHE_API_H

#include <stdio.h>
#include "fcb.h"
#include "free.h"
#include "free_list.h"
#include "fd_table.h"
#include "inode_table.h"
int frame_size;
int split_cache;
int cache_size;
int free_num;  /* number of free blocks */

char* global_cache;
char* buffer;

free_list f_list;
void cache_init(int size);

int cache_open(char* file);

int cache_send(int fd, int client, int n);

#endif
