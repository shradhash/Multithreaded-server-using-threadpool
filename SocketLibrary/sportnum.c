/*
 * sportnum.c -- return the port number of the passed socket 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "socklib.h"

int 
sportnum (s)
    int     s;
{
    struct sockaddr sockname;
    int     len;

    sclrerr ();

    len = sizeof (sockname);
    if (getsockname (s, &sockname, &len) < 0)
    {
	serrno = SE_SYSERR;
	sename = "getsockname";
	return -1;
    }
    if (sockname.sa_family != AF_INET)
    {
	serrno = SE_UNKAF;
	return -1;
    }
    return ((struct sockaddr_in *) (&sockname))->sin_port;
}
