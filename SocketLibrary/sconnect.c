/*
 * sconnect.c -- return a socket connected to a remote address 
 *
 * The function takes strings for the remote host name and remote port.  If the
 * host name starts with a digit, then it is assumed to be an internet
 * dot-style address.  If the service starts with a digit, it is assumed to
 * be a port otherwise it is looked up as a service name. 
 *
 * This is hard-wired to do a stream TCP/IP connection.  If anything bad happens
 * it perror()s and exits.  Maybe later it will return -1 and set some global
 * error context. 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "socklib.h"

int 
sconnect (hostname, servicename)
    char   *hostname;
    char   *servicename;
{
    struct sockaddr_in remote;
    struct sockaddr_in local;
    int     s;
    int     protonum;

    sclrerr ();

    if (make_inetaddr (hostname, servicename, &remote) < 0)
	return -1;

    if ((protonum = protonumber ("tcp")) < 0)
	return -1;

    if ((s = socket (PF_INET, SOCK_STREAM, protonum)) < 0)
    {
	serrno = SE_SYSERR;
	sename = "socket";
	return -1;
    }
    
    if (make_inetaddr ((char *) 0, (char *) 0, &local) < 0)
	return -1;

    if (bind (s, &local, sizeof (local)) < 0)
    {
	serrno = SE_SYSERR;
	sename = "bind";
	return -1;
    }
    if (connect (s, &remote, sizeof (remote)) < 0)
    {
	serrno = SE_SYSERR;
	sename = "connect";
	return -1;
    }
    return s;
}
