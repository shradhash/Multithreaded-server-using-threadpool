/*
 * saccpet.c -- accept a connection on a socket 
 *
 * This should have some way to tell you who it was from, but, the calling code
 * doesn't care; it just doesn't want to have sys/socket.h and all that crap
 * included. 
 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
/*#include <unistd.h>*/
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "socklib.h"

#ifdef _AIX
#include <sys/select.h>
#endif

extern int errno;

int 
saccept (s)
    int     s;
{
    struct sockaddr_in from;
    int     fromlen;
    int     ns;

    sclrerr ();

    fromlen = sizeof (from);
    if ((ns = accept (s, &from, &fromlen)) < 0)
    {
	serrno = SE_SYSERR;
	sename = "accept";
	return -1;
    }
    return ns;
}

int test_accept(int socket_listen, int *returnedSocket)
{
   /* returns -1 for error, 0 for block, 1 for success */

   fd_set fdset;
   static struct timeval timeout = {0, 0};  /* For a poll */
   int    val, socket_talk;

   FD_ZERO(&fdset);
   FD_SET(socket_listen, &fdset);
   val = select(FD_SETSIZE, &fdset, NULL, NULL, &timeout);
/* or could block on select... be nice!
   val = select(FD_SETSIZE, &fdset, NULL, NULL, NULL); */

   if ((val == 0) || (val == -1))
     return val;

   /* Is ready for an accept */
   socket_talk = saccept(socket_listen);
   if (socket_talk < 0)
   {
      if ((errno == EWOULDBLOCK) || (errno == EAGAIN))
         return 0;
      sperror("test_accept failed");
      exit(1);
   }
   *returnedSocket = socket_talk;
   return 1;  /* SUCCESS */
}
