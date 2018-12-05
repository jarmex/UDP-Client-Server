/************************************************************
 listen.c
 
 
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

int main(int argc, char *argv[])
{
  char buf[512];
  char host[64];
  char str[64];
  int s, p, fp, rc, len, port;
  socklen_t childlen;
  struct hostent *hp, *ihp;
  struct sockaddr_in sin, incoming;
  char *hostaddrp; /* dotted decimal host addr string */

  // static response to the client
  char *response = "Received message";
  /* read port number from command line */
  if (argc < 2)
  {
    fprintf(stderr, "Usage: %s <port-number>\n", argv[0]);
    exit(1);
  }
  port = atoi(argv[1]);

  /* fill in hostent struct for self */
  gethostname(host, sizeof host);
  hp = gethostbyname(host);
  if (hp == NULL)
  {
    fprintf(stderr, "%s: host not found (%s)\n", argv[0], host);
    exit(1);
  }
  // char * szLocalIP;
  // szLocalIP = inet_ntoa (*(struct in_addr *)*hp->h_addr_list);
  // printf("Server listing on %s.%d", szLocalIP, port);

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
  // to bind to any of the interface on the server use
  // sin.sin_addr.s_addr = INADDR_ANY; 
  // to bind to a particular IP Address
  memcpy(&sin.sin_addr, hp->h_addr_list[0], hp->h_length);

  /* bind socket s to address sin */
  rc = bind(s, (struct sockaddr *)&sin, sizeof(sin));
  if (rc < 0)
  {
    perror("bind:");
    exit(rc);
  }

  /* listen for connections */
  rc = listen(s, 5);
  if (rc < 0)
  {
    perror("listen:");
    exit(rc);
  }

  childlen = sizeof(incoming);
  /* accept connections */
  while (1)
  {
    p = accept(s, (struct sockaddr *)&incoming, &childlen);
    if (p < 0)
    {
      perror("bind:");
      exit(rc);
    }
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
    printf("server established connection with %s (%s)\n",
           ihp->h_name, hostaddrp);

    // printf(">> Connected to %s\n", ihp->h_name);
    // printf("..Connected\n");

    /* read and print strings sent over the connection */
    while (1)
    {
      len = recv(p, buf, 32, 0);
      if (len < 0)
      {
        perror("recv");
        exit(1);
      }
      buf[len] = '\0';

      /* if client indicates it is closing, break out of loop */
      if (!strcmp("close", buf))
        break;

      /* if client sends any other message, print the message,*/
      else
      {
        printf("%s\n", buf);
      }
      // send a response back to the client
      len = send(p, response, strlen(response), 0);
      if (len < 0)
      {
        perror("send:");
        exit(1);
      }
    }
    close(p);
    printf(">> Connection closed\n");
  }
  close(s);
  exit(0);
}

/*........................ end of listen.c ..................................*/
