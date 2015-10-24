/*
 * sprim.c -- some primitives to take the pain out of creating addresses 
 *
 * make_inetaddr(host/inet dot-style, port/servicename, &struct sockaddr_in); 
 *
 * int protonumber(protoname) 
 */

#include <stdio.h>
#include <ctype.h>
#include <errno.h>

extern int errno;
// extern const char *sys_errlist[];

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "socklib.h"

int     serrno;
char   *sename;

int 
make_inetaddr (hostname, servicename, inaddr)
    char   *hostname;
    char   *servicename;
    struct sockaddr_in *inaddr;
{
    struct hostent *host;
    struct servent *service;

    sclrerr ();

    inaddr->sin_family = AF_INET;
    if (hostname == 0)
    {
	inaddr->sin_addr.s_addr = 0;
    } else
    if (isdigit (*hostname))
    {
	inaddr->sin_addr.s_addr = inet_addr (hostname);
    } else
    {
	if ((host = gethostbyname (hostname)) == 0)
	{
            fprintf(stdout, "Unknown host..\n");
	    serrno = SE_UNKHOST;
	    sename = "gethostbyname";
	    return -1;
	}
	if (host->h_addrtype != AF_INET)
	{
            fprintf(stdout, "Unknown af\n");
	    serrno = SE_UNKAF;
	    return -1;
	}
	/*bcopy (host->h_addr, &inaddr->sin_addr.s_addr, host->h_length);*/
        memcpy(&inaddr->sin_addr.s_addr, host->h_addr, host->h_length);
    }  

    if (servicename == 0)
    {
	inaddr->sin_port = 0;
    } else
    if (isdigit (*servicename))
    {
	inaddr->sin_port = htons(atoi (servicename));
    } else
    {
	if ((service = getservbyname (servicename, "tcp")) == 0)
	{
            fprintf(stdout, "Unknown service.\n");
	    serrno = SE_UNKSERV;
	    sename = "getservbyname";
	    return -1;
	}
	inaddr->sin_port = service->s_port;
    }
    return 0;
}

int 
protonumber (protoname)
    char   *protoname;
{
    struct protoent *proto;

    sclrerr ();

    if ((proto = getprotobyname (protoname)) == 0)
    {
	serrno = SE_UNKPROT;
	sename = "getprotobyname";
	return -1;
    }
    return proto->p_proto;
}

void 
sclrerr ()
{
    serrno = SE_NOERR;
    sename = 0;
}

char   *s_errlist[] = {
		       "no error",
		       "system error",
		       "unknown address family",
		       "unknown host",
		       "unknown service",
		       "unknown protocol",
                       "out of memory"
};

void 
sperror (msg)
    char   *msg;
{
    fprintf (stderr, "%s: ", msg);

    if (sename != 0)
	fprintf (stderr, "%s: ", sename);

    if (serrno == SE_SYSERR)
	fprintf (stderr, "%s\n", sys_errlist[errno]);
    else
	fprintf (stderr, "%s\n", s_errlist[serrno]);
}
