/************************************************************
   File:         dataspeak.c
  Description:   A UDP client application that receive input from the user
                 and send it to the UDP server. It receive response back from the 
                 server
                                                         
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
  int sclient, len, port;
  char host[64], str[BUFFER_SIZE];
  char buf[BUFFER_SIZE]; 
  struct sockaddr_in sin;
  struct hostent *hp;
  socklen_t sinlen;

  /* read host and port number from command line */
  if (argc != 3)
  {
    fprintf(stderr, "Usage: %s <host-name>. <port-number>\n", argv[0]);
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

  /* use address family INET and Datagram sockets (UDP) */
  sclient = socket(AF_INET, SOCK_DGRAM, 0);
  if (sclient < 0)
  {
    perror("Error opening socket:");
    exit(sclient);
  }

  /* set up the address and port */
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);

  sinlen = sizeof(sin);

  printf("Enter a message to send to server: \n");
  /* read a string from the terminal and send on socket */
  while (fgets(str, BUFFER_SIZE, stdin) != NULL)
  {
    len = sendto(sclient, str, strlen(str), 0, (struct sockaddr *)&sin, sizeof(sin));
    if (len < 0)
    {
      perror("Error in sendto:");
      exit(1);
    }
    // print the server's reply
    len = recvfrom(sclient, buf, strlen(buf), 0, (struct sockaddr *)&sin, &sinlen);
    if (len < 0)
    {
      perror("Error in recvfrom: ");
      exit(1);
    }
    // print the response to screen
    buf[len] = '\0';
    printf("%s\n", buf);
    if (strcmp(str, "close\n") == 0) {
      break;
    }
  }

  /* when finished sending, tell host you are closing and close */
  close(sclient);
  exit(0);
}

/*........................ end of dataspeak.c ....................................*/
