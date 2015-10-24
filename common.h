/**
 * common.h, copyright 2001 Steve Gribble
 *
 * Declares some functions and constants that are useful
 * to both the client and server.
 */

#define REQUEST_SIZE 10
#define RESPONSE_SIZE 10

int correct_read(int s, char *data, int len);
int correct_write(int s, char *data, int len);
