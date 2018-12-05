/************************************************************
  File:          datalisten.c
  Description:   A UDP Server to receive message from client and send message.
                  back to the client
                                                         
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 512

int main(int argc, char *argv[])
{
  // read the port number to listen to
  char buf[BUFFER_SIZE];
  int serverid, recvlen, port;
  struct sockaddr_in sin, incoming;
  struct hostent *hp, *ihp;
  char *hostaddrp; /* dotted decimal host addr string */
  socklen_t len;
  /* read port number from command line */
  if (argc < 2)
  {
    fprintf(stderr, "Usage: %s <port-number>\n", argv[0]);
    exit(1);
  }
  port = atoi(argv[1]);

  /* use address family INET and STREAMing sockets (TCP) */
  serverid = socket(AF_INET, SOCK_DGRAM, 0);
  if (serverid < 0)
  {
    perror("socket:");
    exit(serverid);
  }

  /* set up the address and port */
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  // to bind to any of the interface on the server use
  sin.sin_addr.s_addr = INADDR_ANY;

  /* bind socket s to address sin */
  if (bind(serverid, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    perror("bind failed:");
    // exit application
    exit(EXIT_FAILURE);
  }
  // print a message to the screen indicating the server is running
  printf("UDP Server listing on port %d\n", port);

  // static message to reply to clients
  char *response = "RCV:";

  /* receive message from the client */
  len = sizeof(incoming);
  while (1)
  {
    recvlen = recvfrom(serverid, (char *)buf, BUFFER_SIZE, 0, (struct sockaddr *)&incoming, &len);
    if (recvlen < 0)
    {
      perror("recvfrom");
      exit(1);
    }
    /* get the client who sent the message */
    ihp = gethostbyaddr((char *)&incoming.sin_addr,
                        sizeof(struct in_addr), AF_INET);
    if (ihp == NULL)
    {
      perror("ERROR on gethostbyaddr");
    }
    hostaddrp = inet_ntoa(incoming.sin_addr);
    if (hostaddrp == NULL)
    {
      perror("ERROR on inet_ntoa\n");
    }
    printf("server received datagram from %s (%s)\n", ihp->h_name, hostaddrp);

    buf[recvlen] = '\0';
    printf("server received: %s\n", buf);
    // send a confirmation message to the client
    sendto(serverid, (const char *)response, strlen(response), 0, (const struct sockaddr *)&incoming, len);
    if (strcmp(buf, "close\n") == 0)
    {
      break;
    }
  }
  close(serverid);
  return 0;
}