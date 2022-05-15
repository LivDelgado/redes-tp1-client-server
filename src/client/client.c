#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

static const int BUFSIZE = 500; // tamanho máximo da mensagem

void printErrorAndExit(char *errorMessage)
{
   puts(errorMessage);
   exit(1);
}

int setupClientSocket(char *serverIpAddress, char *serverPort)
{
   // Tell the system what kind(s) of address info we want
   struct addrinfo addressCriteria;                      // Criteria for address match
   memset(&addressCriteria, 0, sizeof(addressCriteria)); // Zero out structure
   addressCriteria.ai_family = AF_UNSPEC;                // v4 or v6 is OK
   addressCriteria.ai_socktype = SOCK_STREAM;            // Only streaming sockets
   addressCriteria.ai_protocol = IPPROTO_TCP;            // Only TCP protocol

   // Get address(es)
   struct addrinfo *possibleServerAddresses; // Holder for returned list of server addrs
   int gotPossibleSocketAddresses = getaddrinfo(serverIpAddress, serverPort, &addressCriteria, &possibleServerAddresses);
   if (gotPossibleSocketAddresses != 0)
   {
      printErrorAndExit("getaddrinfo() failed");
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
      printErrorAndExit("Failed to setup client socket.");
   }

   return clientSocket;
}

void sendMessageToServer(int clientSocket, char *message, size_t messageLength)
{
   ssize_t numberOfBytesBeingSent = send(clientSocket, message, messageLength, 0);
   if (numberOfBytesBeingSent < 0)
   {
      printErrorAndExit("failed to send message.");
   }
   else if (numberOfBytesBeingSent != messageLength)
   {
      printErrorAndExit("could not send the correct message to the server.");
   }
}

void receiveResponseFromServer(int clientSocket, size_t messageLength)
{
   unsigned int totalBytesReceived = 0;
   ssize_t numberOfBytesReceived;
   while (totalBytesReceived < messageLength)
   {
      char buffer[BUFSIZE]; // I/O buffer
      // Receive up to the buffer size (minus 1 to leave space for a null terminator) bytes from the sender
      numberOfBytesReceived = recv(clientSocket, buffer, BUFSIZE - 1, 0);

      if (numberOfBytesReceived < 0)
      {
         printErrorAndExit("failed to receive response from server.");
      }
      else if (numberOfBytesReceived == 0)
      {
         printErrorAndExit("connection closed before intended!");
      }

      totalBytesReceived += numberOfBytesReceived;
      buffer[numberOfBytesReceived] = '\0';

      printf("< ");
      puts(buffer); // imprime resposta do servidor
   }
}

void communicateWithServer(int clientSocket)
{
   while (1)
   {
      char *message = NULL; // create new message
      size_t messageLength;
      
      printf("> ");
      getline(&message, &messageLength, stdin); // get the message from user input

      // saída para desligar o cliente sem desligar o servidor
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
   // porta padrão é 51511
   if (argc != 3)
   {
      printErrorAndExit("Invalid arguments. To run the client: client <server address> <server port>");
   }

   char *serverIpAddress = argv[1]; // o primeiro argumento é o endereço do servidor
   char *serverPort = argv[2];      // o segundo argumento é a porta de conexão do socket no servidor

   int clientSocket = setupClientSocket(serverIpAddress, serverPort);

   communicateWithServer(clientSocket);
}