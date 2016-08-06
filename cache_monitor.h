/*
* File: cache_api.h
* Author: Daniel
* Purpose: Define the cache file system API
**/

#ifndef CACHE_MONITOR_H
#define CACHE_MONITOR_H

#include <stdio.h>
#include "cache_api.h"

extern int cache_writer(char* file);

extern int cache_reader(int fd, int client, int n);

#endif

