/*
Author: Martin Edmunds
Class: CS 372 Network
Date: 01/29/20

Description:


*/
#define _POSIX_C_SOURCE 200809L
#define BUFFER_SIZE 1024
#define SEND_SIZE 1024
#define NAME_SIZE 10

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/ioctl.h>

void start_chat(int socketFD);
int CreateSocketFD(int port);


/*
argv[1] = port number
*/
int main(int argc, char** argv)
{

    int socketFD = CreateSocketFD(atoi(argv[1]));

    start_chat(socketFD);

    close(socketFD);

    return 0;

}


void start_chat(int socketFD)
{
  //create input buffer for messages
  char* input_buffer = (char*)malloc(BUFFER_SIZE);
  size_t input_buffer_size = BUFFER_SIZE;
  memset(input_buffer, 0, BUFFER_SIZE);

  //create name buffer for client name
  char* client_name = (char*)malloc(NAME_SIZE + 1);
  size_t client_buffer_size = NAME_SIZE + 1;
  memset(client_name, 0, NAME_SIZE+1);

  //create receive buffer
  char* recv_buffer = (char*)malloc(BUFFER_SIZE + 1);
  size_t recv_buffer_size = BUFFER_SIZE + 1;
  memset(recv_buffer, 0, BUFFER_SIZE + 1);

  //get handle from user
  printf("Please enter name for handle. [10 chars max]\n");
  fgets(client_name, NAME_SIZE+1, stdin);

  //get rid of newline
  client_name[strlen(client_name)-1] = 0;

  //send name to host
  send(socketFD, client_name, strlen(client_name), 0);

  while(1)
  {
    //0 input buffer and receive buffer
    memset(input_buffer, 0, BUFFER_SIZE);
    memset(recv_buffer, 0, BUFFER_SIZE + 1);

    //receive message from host
    recv(socketFD, recv_buffer, recv_buffer_size, 0);
    printf("%s", recv_buffer);

    //get user input
    printf("\n%s> ", client_name);
    fgets(input_buffer, BUFFER_SIZE, stdin);
    input_buffer[strlen(input_buffer) - 1] = 0;

    //send message to host
    send(socketFD, input_buffer, strlen(input_buffer), 0);
  }

}



/*
Creates a socket file descriptor for use in file I/O type communication.
This function currently only creates a socket on 'localhost' with the port number
specified in the argument.

Returns a file-descriptor-like socket that can be used with
send and recv
*/
int CreateSocketFD(int port)
{
    int socketFD, portNumber, bytesWritten, bytesRead;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;

    memset((char*)&serverAddress, '\0', sizeof(serverAddress));
    portNumber = port; //convert port number

    serverAddress.sin_family = AF_INET; //network socket type
    serverAddress.sin_port = htons(portNumber); //convert port to big endian
    serverHostInfo = gethostbyname("localhost");

    if(serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }

    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr_list[0], serverHostInfo->h_length); //copy in server address

    //set up socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); //create socket
    if(socketFD < 0){ fprintf(stderr, "Error creating socket"); }

    if(connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
    {
        fprintf(stderr, "CLIENT: Error connecting on port: %i\n", portNumber);
        perror("\n");
        exit(2);
    }

    return socketFD;
}