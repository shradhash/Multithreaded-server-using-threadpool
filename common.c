/**
 * common.c, copyright 2001 Steve Gribble
 *
 * This file contains some common utility functions that
 * both the client and server can use.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "common.h"

/**
 * This function writes back a response over the socket
 * to the client.  This function is thread safe.
 */
void  send_response(int fd, char *response, int response_length) {
  correct_write(fd, response, response_length);
}


/**
 * A utility function for reading a fixed number of bytes
 * from a socket.  This function is thread safe.
 */
int correct_read(int s, char *data, int len)
{
  int sofar, ret;
  char *tmp;

  sofar = 0;
  while(sofar != len) {
    tmp = data + (unsigned long) sofar;
    ret = read(s, tmp, len-sofar);
    if (ret <= 0) {
      if (! ((ret == -1) && ((errno == EAGAIN) || (errno == EINTR))) )
        return ret;
    } else
      sofar += ret;
  }
  return len;
}

/**
 * A utility function for writing a fixed number of bytes
 * over a network socket.  This function is thread safe.
 */
int correct_write(int s, char *data, int len)
{
  int sofar, ret;
  char *tmp;

  if (len == -1) 
    len = strlen(data);
  
  sofar = 0;
  while (sofar != len) {
    tmp = data + (unsigned long) sofar;
    ret = write(s, tmp, len-sofar);
    if (ret <= 0) {
      if (! ((ret == -1) && ((errno == EAGAIN) || (errno == EINTR))) )
        return ret;
    } else
      sofar += ret;
  }
  return len;
}
