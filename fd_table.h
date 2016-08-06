/*
* File: fd_table.h
* Author: Daniel
**/

#ifndef FD_TABLE_H
#define FD_TABLE_H

#include <stdio.h>


typedef struct _file_dt{
  int inx;
  int cfd;
  int size;  /* how many file descriptor do I have? */
  int left;
  /* file offset */
  int seek_cur;
} file_dt;

#endif
