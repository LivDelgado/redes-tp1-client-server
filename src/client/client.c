#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFSIZE 500 // max message size

// prints the message sent in the parameter and exit with error status
void printErrorAndExit(char *errorMessage)
{
   puts(errorMessage);
   exit(1);
}

int setupClientSocket(char *serverIpAddress, char *serverPort)
{
   // tell the system what kind(s) of address info we want
   struct addrinfo addressCriteria;                      // Criteria for address match
   memset(&addressCriteria, 0, sizeof(addressCriteria)); // Zero out structure
   addressCriteria.ai_family = AF_UNSPEC;                // v4 or v6 is OK
   addressCriteria.ai_socktype = SOCK_STREAM;            // Only streaming sockets
   addressCriteria.ai_protocol = IPPROTO_TCP;            // Only TCP protocol

   // get address(es)
   struct addrinfo *possibleServerAddresses; // Holder for returned list of server addrs
   int gotPossibleSocketAddresses = getaddrinfo(serverIpAddress, serverPort, &addressCriteria, &possibleServerAddresses);
   if (gotPossibleSocketAddresses != 0)
   {
      printErrorAndExit("ERROR: getaddrinfo() failed");
   }

   int clientSocket = -1;
   for (struct addrinfo *address = possibleServerAddresses; address != NULL; address = address->ai_next)
   {
      // Create a reliable, stream socket using TCP
      clientSocket = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
      if (clientSocket < 0)
      {
         continue; // Socket creation failed; try next address
      }

      // Establish the connection to the echo server
      if (connect(clientSocket, address->ai_addr, address->ai_addrlen) == 0)
      {
         break; // Socket connection succeeded; break and return socket
      }

      close(clientSocket); // could not connect to that address, trying next one
      clientSocket = -1;
   }

   freeaddrinfo(possibleServerAddresses); // free address info allocated

   if (clientSocket < 0)
   {
      printErrorAndExit("ERROR: Failed to setup client socket.");
   }

   return clientSocket;
}

void sendMessageToServer(int clientSocket, char *message, size_t messageLength)
{
   ssize_t numberOfBytesBeingSent = send(clientSocket, message, messageLength, 0);
   if (numberOfBytesBeingSent < 0)
   {
      printErrorAndExit("ERROR: failed to send message.");
   }
   else if (numberOfBytesBeingSent != messageLength)
   {
      printErrorAndExit("ERROR: could not send the correct message to the server.");
   }
}

void receiveResponseFromServer(int clientSocket, size_t messageLength)
{
   unsigned int totalBytesReceived = 0;
   ssize_t numberOfBytesReceived;
   char buffer[BUFSIZE] = ""; // I/O buffer

   // Receive up to the buffer size bytes from the sender
   numberOfBytesReceived = recv(clientSocket, buffer, BUFSIZE - 1, 0);

   if (numberOfBytesReceived < 0)
   {
      printErrorAndExit("ERROR: failed to receive response from server.");
   }
   else if (numberOfBytesReceived == 0 )
   {
      printErrorAndExit("WARNING: server closed the connection!");
   }

   totalBytesReceived += numberOfBytesReceived;
   buffer[numberOfBytesReceived] = '\0';

   printf("< %s", buffer); // prints message received from the server
}

void communicateWithServer(int clientSocket)
{
   while (1)
   {
      char *message = NULL; // create new message
      size_t messageLength;

      printf("> ");
      getline(&message, &messageLength, stdin); // get the message from user input

      // command to shut the client (and the client only) down
      if (strcmp(message, "close client\n") == 0)
      {
         close(clientSocket);
         exit(0);
      }

      messageLength = strlen(message); // get the message size

      sendMessageToServer(clientSocket, message, messageLength);

      receiveResponseFromServer(clientSocket, messageLength);
   }
}

int main(int argc, char *argv[])
{
   if (argc != 3)
   {
      printErrorAndExit("ERROR: Invalid arguments. To run the client: client <server address> <server port>");
   }

   char *serverIpAddress = argv[1]; // first argument is server address
   char *serverPort = argv[2];      // second argument is server port

   int clientSocket = setupClientSocket(serverIpAddress, serverPort);

   communicateWithServer(clientSocket);
}