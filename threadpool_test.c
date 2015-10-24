/**
 * threadpool_test.c, copyright 2001 Steve Gribble
 *
 * Just a regression test for the threadpool code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "threadpool.h"

extern int errno;

void dispatch_to_me(void *arg) {
  int seconds = (int) arg;

  fprintf(stdout, "  in dispatch %d\n", seconds);
  sleep(seconds);
  fprintf(stdout, "  done dispatch %d\n", seconds);
}

int main(int argc, char **argv) {
  threadpool tp;

  tp = create_threadpool(6);

  fprintf(stdout, "**main** dispatch 3\n");
  dispatch(tp, dispatch_to_me, (void *) 3);
  fprintf(stdout, "**main** dispatch 6\n");
  dispatch(tp, dispatch_to_me, (void *) 6);
  fprintf(stdout, "**main** dispatch 7\n");
  dispatch(tp, dispatch_to_me, (void *) 7);

  fprintf(stdout, "**main** done first\n");
  sleep(20);
  fprintf(stdout, "\n\n");

  fprintf(stdout, "**main** dispatch 3\n");
  dispatch(tp, dispatch_to_me, (void *) 3);
  fprintf(stdout, "**main** dispatch 6\n");
  dispatch(tp, dispatch_to_me, (void *) 6);
  fprintf(stdout, "**main** dispatch 7\n");
  dispatch(tp, dispatch_to_me, (void *) 7);

  fprintf(stdout, "**main done second\n");
  sleep(20);
  exit(-1);
}

