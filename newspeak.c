/************************************************************
   File:          newspeak.c
  Description:   A TCP client application that connect to a TCP Server
                  The client read an input from the user and send it to the server
                                                         
  Author:        James Amo      
  Organization:  IUSB, Computer and Information Sciences
  Date:          December 2018
*************************************************************/

/*........................ Include Files ....................................*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 512
int main(int argc, char *argv[])
{
  int s, rc, len, port;
  char host[64], str[128];
  char buf[512];
  struct hostent *hp;
  struct sockaddr_in sin;

  /* read host and port number from command line */
  if (argc != 3)
  {
    fprintf(stderr, "Usage: %s <host-name. <port-number>\n", argv[0]);
    exit(1);
  }

  /* fill in hostent struct */
  hp = gethostbyname(argv[1]);
  if (hp == NULL)
  {
    fprintf(stderr, "%s: host not found (%s)\n", argv[0], host);
    exit(1);
  }
  port = atoi(argv[2]);

  /* create and connect to a socket */

  /* use address family INET and STREAMing sockets (TCP) */
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0)
  {
    perror("socket:");
    exit(s);
  }

  /* set up the address and port */
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);

  /* connect to socket at above addr and port */
  rc = connect(s, (struct sockaddr *)&sin, sizeof(sin));
  if (rc < 0)
  {
    perror("connect:");
    exit(rc);
  }

  /* read a string from the terminal and send on socket */
  while (fgets(str, 128, stdin) != NULL)
  {
    len = send(s, str, strlen(str), 0);
    if (len != strlen(str))
    {
      perror("send");
      exit(1);
    }
    // receive data from the server and print
    len = recv(s, buf, BUFFER_SIZE, 0);
    if (len < 0)
    {
      perror("recv");
      exit(1);
    }
    buf[len] = '\0';
    printf("%s\n", buf);
  }

  /* when finished sending, tell host you are closing and close */
  send(s, "close", 5, 0);
  close(s);
  exit(0);
}

/*........................ end of newspeak.c ....................................*/
