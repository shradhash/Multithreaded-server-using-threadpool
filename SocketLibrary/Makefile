CC=gcc
#CC=cc -Aa -D_HPUX_SOURCE
DOT_O = 
RANLIB = ranlib
ARFLAGS = ru
AR = ar
OBJS=saccept.o sconnect.o slisten.o sportnum.o sprim.o smessages.o
CFLAGS= -g
SRCS= socklib.h saccept.c sconnect.c slisten.c sportnum.c sprim.c smessages.c
HEADER= socklib.h

all: libsock.a

libsock.a: $(OBJS) $(HEADER)
	$(AR) $(ARFLAGS) libsock.a $(OBJS)
	$(RANLIB) libsock.a

saccept.o: saccept.c $(HEADER)
	$(CC) -c $(CFLAGS) -o saccept.o saccept.c

sconnect.o: sconnect.c $(HEADER)
	$(CC) -c $(CFLAGS) -o sconnect.o sconnect.c

slisten.o: slisten.c $(HEADER)
	$(CC) -c $(CFLAGS) -o slisten.o slisten.c

sportnum.o: sportnum.c $(HEADER)
	$(CC) -c $(CFLAGS) -o sportnum.o sportnum.c

sprim.o: sprim.c $(HEADER)
	$(CC) -c $(CFLAGS) -o sprim.o sprim.c

smessages.o: smessages.c $(HEADER)
	$(CC) -c $(CFLAGS) -o smessages.o smessages.c

clean:
	rm -f *.o *~
	rm -f libsock.a
