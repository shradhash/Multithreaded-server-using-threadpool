/*
 * socklib.h 
 */

extern int saccept ();
extern int sconnect ();
extern int slisten ();
extern int sportnum ();

extern char *serror ();
extern char *sename;
extern int serrno;

extern void sclrerr ();
extern void sperror ();

extern int  test_accept();
extern int  incoming_messages();
extern int  get_next_message();
extern int  send_a_message();
extern int  empty_incoming_messages();
extern int  wait_for_message();

/* these return -1 for error, 0 for would block, 1 for would succeed */
extern int test_ready(int s);
extern int test_writey(int s);

int     make_inetaddr ();
int     protonumber ();

#define SE_NOERR	(0)
#define SE_SYSERR	(1)
#define SE_UNKAF	(2)
#define SE_UNKHOST	(3)
#define SE_UNKSERV	(4)
#define SE_UNKPROT	(5)
#define SE_NONMEM       (6)

#define MAXBUFF 2056

