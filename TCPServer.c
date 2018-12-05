/************************************************************
  File:          TCPServer.c
  Description:   The TCPServer concurrent application is a modification of listen.c 
                  which was provided project 1. The TCPServer is a guessing game which can accept 
                  multiple client simultaneously. The client will enter a number to guess 
                  the random number generated on the server. The server respond according 
                  to the guessed number from the client.
                                                         
  Author:        James Amo, IUSB 
  Course:        B438/B538 Fundamental of Computer Networking                      
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
#include <time.h>

// main
int main(int argc, char *argv[])
{
  char buf[512];
  char host[64];
  int s, p, fp, rc, len, port;
  socklen_t childlen;
  struct hostent *hp, *ihp;
  struct sockaddr_in sin, incoming;
  char *hostaddrp; /* dotted decimal host addr string */

  // for random number generation
  time_t t;
  /* Intializes random number generator */
  srand((unsigned)time(&t));
  // number to store a guessing game
  int guessed_number;
  // number of simulataneous connections that can be supported by the application
  int client_socket[30];
  // message to be sent to the client
  char response[256];
  // is welcome message sent to the client
  unsigned int isWelcome = 0;
  // the number the client enter
  int clientnum;

  // process identification
  pid_t pid;

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
  // check if the host information is found
  if (hp == NULL)
  {
    fprintf(stderr, "%s: host not found (%s)\n", argv[0], host);
    exit(1);
  }

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

  // print the local socket address for which the application is bound to!!

  char *szLocalIP;
  szLocalIP = inet_ntoa(*(struct in_addr *)*hp->h_addr_list);
  printf("Server listing on %s.%d\n", szLocalIP, port);

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
    // parent process waiting to accept a new connection
    p = accept(s, (struct sockaddr *)&incoming, &childlen);

    // check if the new connection return error
    if (p < 0)
    {
      perror("bind:");
      exit(rc);
    }
    // returns a structure of type hostent
    ihp = gethostbyaddr((char *)&incoming.sin_addr, sizeof(struct in_addr), AF_INET);
    if (ihp == NULL)
    {
      perror("ERROR on gethostbyaddr");
    }
    hostaddrp = inet_ntoa(incoming.sin_addr);
    if (hostaddrp == NULL)
    {
      perror("ERROR on inet_ntoa\n");
    }
    // print the client connected to the server
    printf("server established connection with %s (%s)\n", ihp->h_name, hostaddrp);

    // create a child process
    pid = fork();
    // check if the child process was created successfully
    if (pid == -1)
    {
      perror("fork() failed");
      continue;
    }
    if (pid > 0)
    {
      close(p); // sock is closed BY PARENT
    }
    else // pid = 0
    {
      guessed_number = rand() % 100;
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
        if (strcmp( buf, "close\n") == 0)
        {
          printf("Closing connection: \n");
          break;
        }
        else /* if client sends any other message, print the message,*/
        {
          printf("CLIENT: %s\n", buf);

          clientnum = atoi(buf);
          if (clientnum == guessed_number)
          {
            strcpy(response, " Horray!!!, YOU HAVE WON!!!!!");
            guessed_number = rand() % 100;
          }
          else if (clientnum > guessed_number)
          {
            sprintf(response, "Your number (%d) is too big. Try again", clientnum);
          }
          else
          {

            if (clientnum == 0 && isWelcome == 0)
            {
              strcpy(response, "Welcome to the guessing game. Enter the guess number between 0 and 100: ");
              isWelcome = 1; // send the welcome message to the client
            }
            else if (clientnum < guessed_number)
            {
              sprintf(response, "Your number (%d) is too small. Try again", clientnum);
            }
            else
            {
              strcpy(response, "Enter number between 0 and 100");
            }
          }

          // send a response back to the client
          printf("SERVER: %s\n", response);
          len = send(p, response, strlen(response), 0);
          if (len < 0)
          {
            perror("send:");
            exit(1);
          }
        }
      }
      // child process closing.
      close(p);
      printf(">> Connection closed\n");

    }
  }
  // close the parent socket
  close(s);
  exit(0);
}

/*........................ end of TCPServer.c ..................................*/
