/*
 *** smessages.c - provides a level of abstraction from the stream sockets.
 ***
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
/*#include <unistd.h>*/
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h>

#ifdef _AIX
#include <sys/select.h>
#endif

#include "socklib.h"

typedef struct mess_list_st {
    int                 s;                       /* socket number */
    char                message[MAXBUFF];       /* actual message */
    int                 complete;  /* 1 if whole message captured */
    struct mess_list_st *next;
} *mess_list, mess_list_el;

mess_list  the_list = NULL;
extern int errno;

void handle(int s, char *message, char *end_of_transmission);
mess_list add_incomplete(int s, char *message);
void clean_list(int s);

int incoming_messages(s)
    int   s;
{
    mess_list   temp;
    int         incomplete = 0, num_messages=0, result;
    char        message[MAXBUFF];
    char        *tmp, *tmpprev;

    sclrerr();

    /* Now must continue to read from socket s until reading is no
       longer possible. */
    result = 1;
    while (result > 0)
    {
        result = test_ready(s);
        if (result <= 0)
           break;

        result = read(s, message, MAXBUFF-1);
        if (result > 0)
	  message[result] = '\0';   /* To make sure is null-term */
        if (result == 0)
        {
            clean_list(s);
            return -1;
        }
        if (result == -1)
        {
           if ((errno != EAGAIN) && (errno != EWOULDBLOCK))
           {
              serrno = SE_SYSERR;
              sename = "incoming messages";
              sperror("incoming messages read");
              exit(-1);
           }
        }
        else
	{
          /* Have a message.  Split into chunks and sent off to handler */
          tmp = tmpprev= message;
          while (tmp <= message + result) 
          {
             if ((*tmp == '\0') && (tmp != tmpprev))
             {
                handle(s, tmpprev, message + result - 1);
                tmpprev = tmp + 1;
             }
             if ((tmp == tmpprev) && (*tmp == '\0'))
               tmpprev += 1;
             tmp += 1;
          }
	}
    }

    /* Find out how many messages there are for s on the queue.  If
       we come across a message with complete = 0, it is the last
       message on the queue, but doesn't (yet) count towards the
       total. */

    temp = the_list;
    while (temp != NULL)
    {
      if (temp->s == s)
      {
        if (temp->complete == 1)
          num_messages += 1;
        else
        {
            incomplete = 1;
            break;
        }
      }
      temp = temp->next;
    }

    return num_messages;
}

int get_next_message(s, c)
    int    s;
    char   c[MAXBUFF];
{
    mess_list temp, prev;
    int       result;

    sclrerr();

    if ((result = incoming_messages(s)) == 0)
    {
         sprintf(c, "");
         return 0;
    }
    if (result < 0)
      return result;

    /* Is at least one message for s on queue.  Copy the message
       into c, and destroy the linked list element */
    
    temp = prev = the_list;
    if ((temp->s == s) && (temp->complete == 1))
    {
         strcpy(c, temp->message);
         the_list = temp->next;
         free(temp);
         return 1;
    }
    while (temp != NULL)
    {
      if ((temp->s == s) && (temp->complete == 1))
      {
         strcpy(c, temp->message);
         prev->next = temp->next;
         free(temp);
         return 1;
      }
      prev = temp;
      temp = temp->next;
    }
    return 0;
}

int send_a_message(s, c)
    int   s;
    char  *c;
{
    int result;

    sclrerr();
    
    result = test_writey(s);
    if (result == 0)
       return 0;
    if (result > 0)
       result = write(s, c, strlen(c)+1);
    if (result != strlen(c)+1)
    {
       if ((result == -1) && (errno == EWOULDBLOCK))
          return 0;
       if (result == 0)
       {
          /* Socket died. */
          clean_list(s);
          return -1;
       }
       if (result == -1)
       {
            serrno = SE_SYSERR;
            sename = "send a message";
            sperror("send a message write");
            exit(-1);
       }
       /* Hmm.. */
       fprintf(stderr, "Warning, could only write %d bytes of data.\n", result);
    }
    return result;
}

void handle(int s, char *message, char *end_of_transmission)
{
    int incomplete_add = 0, incomplete = 0;
    mess_list  temp;

    if ( (message + strlen(message)) > end_of_transmission)
       incomplete = 1;

    temp = the_list;
    while (temp != NULL)
    {
        if (temp->s == s)
        {
          if (temp->complete != 1)
          {
             incomplete_add = 1;
             break;
          }
        }
        temp = temp->next;
    }
    if (incomplete_add == 1)
    {
        if (incomplete == 0)
           temp->complete = 1;
        else
           temp->complete = 0;
        strcat(temp->message, message);
    }
    else
    {
       temp = add_incomplete(s, "");
       strcat(temp->message, message);
       if (incomplete == 0)
          temp->complete = 1;
       else
          temp->complete = 0;
    }
}

void clean_list(int s)
{
    mess_list temp, prev;

    temp = prev = the_list;

    if (temp == NULL)
      return;

    while (temp->s == s)
    {
       prev = temp;
       temp = temp->next;
       free(prev);
       the_list = temp;
       if (the_list == NULL)
          return;
    }

    while (temp->next != NULL)
    {
        prev = temp;
        temp = temp->next;
        if (temp->s == s)
        {
           prev->next = temp->next;
           prev = temp->next;
           free(temp);
           temp = prev;
           if (temp == NULL)
              return;
        }
    }
}
mess_list add_incomplete(int s, char *message)
{
    mess_list   temp;

    if (the_list == NULL)
    {
        the_list = (mess_list) malloc(sizeof(mess_list_el));
        if (the_list == NULL)
        {
              serrno = SE_NONMEM;
              sename = "add_incomplete";
              sperror("Yipe!");
              exit(-1);
        }
        the_list->next = NULL;
        the_list->s = s;
        the_list->complete = 0;
        strcpy(the_list->message, message);
        return the_list;
    }

    temp = the_list;
    while (temp->next != NULL)
      temp = temp->next;

    temp->next = (mess_list) malloc(sizeof(mess_list_el));
    if (temp->next == NULL)
    {
       serrno = SE_NONMEM;
       sename = "add_incomplete";
       sperror("Yipe!!");
       exit(-1);
    }
    temp = temp->next;
    temp->next = NULL;
    temp->s = s;
    temp->complete = 0;
    strcpy(temp->message, message);
    return temp;
}

int test_ready(int s)
{
  /* returns -1 for error, 0 for block, 1 for success */

   fd_set fdset;
   static struct timeval timeout = {0, 0};  /* For a poll */
   int    val, socket_talk;

   FD_ZERO(&fdset);
   FD_SET(s, &fdset);
   val = select(FD_SETSIZE, &fdset, NULL, NULL, &timeout);

   if (val > 0)
      return 1;

   return val;
}

int wait_for_message(int s, int time)
{
  /* returns 0 for none_arrived,  1 for something arrived */
  fd_set fdset;
  static struct timeval timeout;
  int    val, socket_talk;

  FD_ZERO(&fdset);
  FD_SET(s, &fdset);
  timeout.tv_sec = time;  timeout.tv_usec = 0;
  if (time < 0)
    val = select(FD_SETSIZE, &fdset, NULL, NULL, NULL);  /* block */
  else
    val = select(FD_SETSIZE, &fdset, NULL, NULL, &timeout); /* timeout */

  if (val > 0)
    return 1;
  return 0;
}

int test_writey(int s)
{
  /* returns -1 for error, 0 for block, 1 for success */

   fd_set fdset;
   static struct timeval timeout = {0, 0};  /* For a poll */
   int    val, socket_talk;

   FD_ZERO(&fdset);
   FD_SET(s, &fdset);
   val = select(FD_SETSIZE, NULL, &fdset, NULL, &timeout);

   if (val > 0)
      return 1;

   return val;
}
int  empty_incoming_messages(s)
     int  s;
{
   mess_list temp, prev;
   int       result;

   sclrerr();

    if ((result = incoming_messages(s)) == 0)
         return 1;

    if (result < 0)
      return result;

    /* Is at least one message for s on queue; destroy the element */

    while (the_list->s == s)
    {
       temp = prev = the_list;
       the_list = temp->next;
       free(temp);
       if (the_list == NULL)
          return;
    }
    while (temp != NULL)
    {
      if (temp->s == s)
      {
         prev->next = temp->next;
         free(temp);
         return 1;
      }
      prev = temp;
      temp = temp->next;
    }
    return 1;
}
