/*
 * slisten.c -- create a socket that will be listening for connections 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "socklib.h"

int 
slisten (servicename)
    char   *servicename;
{
    struct sockaddr_in inaddr;
    int     s;
    int     protonum;

    sclrerr ();

    if ((protonum = protonumber ("tcp")) < 0)
	return -1;

    if ((s = socket (PF_INET, SOCK_STREAM, protonum)) < 0)
    {
	serrno = SE_SYSERR;
	sename = "socket";
	return -1;
    }
    if (make_inetaddr ((char *) 0, servicename, &inaddr) < 0)
	return -1;

    if (bind (s, &inaddr, sizeof (inaddr)) < 0)
    {
	serrno = SE_SYSERR;
	sename = "bind";
	return -1;
    }
    if (listen (s, 3) < 0)
    {
	serrno = SE_SYSERR;
	sename = "listen";
	return -1;
    }
    return s;
}
