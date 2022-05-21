#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "processor.h"
#include "monitor.h"

// constants related to the protocol defined
#define BUFSIZE 500                         // message max size
static const int MAX_PENDING = 5;           // max pending connection requests
static const int DEFAULT_PORT = 51511;      // default serverport
static const int ALLOW_IPV6_CONNECTION = 1; // flag to control the ability to connect with ipv6 address

// prints the message sent in the parameter and exit with error status
void printErrorAndExit(char *errorMessage)
{
    puts(errorMessage);
    exit(1);
}

// validate address type and exit in case of failure
// the address type should be either v4 or v6
void validateAddressType(char *addressType)
{
    if (strcmp(addressType, "v4") != 0 && strcmp(addressType, "v6") != 0)
    {
        printErrorAndExit("ERROR: Invalid address type. It should be either v4 or v6.");
    }
    if (strcmp(addressType, "v6") != 0 && !ALLOW_IPV6_CONNECTION)
    {
        printErrorAndExit("ERROR: ipv6 is not supported.");
    }
}

int createIpv4TcpSocket()
{
    int serverSocket;
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printErrorAndExit("ERROR: Failed to create ipv4 socket.");
    }

    return serverSocket;
}

int createIpv6TcpSocket()
{
    int serverSocket;
    if ((serverSocket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printErrorAndExit("ERROR: Failed to create ipv6 socket.");
    }

    return serverSocket;
}

int createTcpSocket(int addressIsV6)
{
    if (addressIsV6)
    {
        return createIpv6TcpSocket();
    }
    else
    {
        return createIpv4TcpSocket();
    }
}

void bindToLocalIpv4Address(int serverSocket, int port)
{
    // creates local address struct
    struct sockaddr_in serverAddress;                  // local address
    memset(&serverAddress, 0, sizeof(serverAddress));  // clear out structure
    serverAddress.sin_family = AF_INET;                // IPv4
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // any incoming interface
    serverAddress.sin_port = htons(port);              // local port

    // bind to local address
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printErrorAndExit("ERROR: Bind failed!");
        close(serverSocket);
    }
}

void bindToLocalIpv6Address(int serverSocket, int port)
{
    // creates local address struct
    struct sockaddr_in6 serverAddress;                // local address
    memset(&serverAddress, 0, sizeof(serverAddress)); // clear out structure
    serverAddress.sin6_family = AF_INET6;             // IPv6
    serverAddress.sin6_addr = in6addr_any;            // any incoming interface
    serverAddress.sin6_port = htons(port);            // local port

    // bind to local address
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printErrorAndExit("ERROR: Bind failed!");
        close(serverSocket);
    }
}

void bindToLocalAddress(int serverSocket, int port, int addressIsV6)
{
    if (addressIsV6)
    {
        bindToLocalIpv6Address(serverSocket, port);
    }
    else
    {
        bindToLocalIpv4Address(serverSocket, port);
    }
}

void setTheServerToListen(int serverSocket, int port)
{
    printf("DEBUG: Trying to set the server to listen at port %i.\n", port);

    if (listen(serverSocket, MAX_PENDING) < 0)
    {
        printErrorAndExit("ERROR: Server could not listen.");
        close(serverSocket);
    }

    printf("INFO: Server listening at port %i\n", port);
}

void handleClient(int serverSocket, int clientSocket)
{
    steel *steelBuilding = newSteelIndustryBuilding(); // new structure for each client/connection

    char buffer[BUFSIZE] = "";

    ssize_t numberOfBytesReceived;
    char responseToClient[BUFSIZE] = "";

    // Send received string and receive again until end of stream
    do
    {
        // Receive message from client
        puts("DEBUG: receiving message from client.");
        numberOfBytesReceived = recv(clientSocket, buffer, BUFSIZE, 0);
        if (numberOfBytesReceived < 0)
        {
            printErrorAndExit("ERROR: recv() failed");
        }
        else if (numberOfBytesReceived == 0)
        {
            puts("INFO: Client closed the connection!");
            close(clientSocket);
            return;
        }

        printf("< %s", buffer); // prints client message

        // process message and return the string to send back to the client
        char message[BUFSIZE] = "";

        // copy into a new array to avoid messing something
        // remove last character because it is a line break
        strncpy(message, buffer, strlen(buffer) - 1);

        char *messageReturn = processMessage(message, steelBuilding); // process message
        memset(&message, 0, sizeof(message));                         // clean message up

        // if some error happened, close connection and leave the handler
        if (messageReturn == NULL)
        {
            close(clientSocket);
            puts("ERROR: Unknown command sent by client. Connection closed.");
            return;
        }

        if (strcmp(messageReturn, "kill") == 0) // received kill command, should shut down
        {
            puts("WARNING: kill command. shutting server down!!");
            close(clientSocket);
            close(serverSocket);
            free(steelBuilding);
            exit(0);
        }

        // send response to client
        strcat(responseToClient, messageReturn);
        strcat(responseToClient, "\n");
        ssize_t numberOfBytesSent = send(clientSocket, responseToClient, strlen(responseToClient), 0);
        printf("> %s", responseToClient);

        if (numberOfBytesSent < 0) // no message was sent
        {
            printErrorAndExit("ERROR: send() failed");
        }

        // reset buffer
        memset(&buffer, 0, sizeof(buffer));
        // reset response to client
        memset(&responseToClient, 0, sizeof(responseToClient));

    } while (numberOfBytesReceived > 0);

    close(clientSocket); // Close client socket
    free(steelBuilding); // Free the memory allocated for the structure created for this client
}

void handleIpv4(int serverSocket)
{
    while (1)
    {
        struct sockaddr_in clientAddress;

        socklen_t clientAddressLength = sizeof(clientAddress);

        // Wait for a client to connect
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (clientSocket < 0)
        {
            printErrorAndExit("ERROR: Failed to accept connection from client.");
        }

        puts("INFO: A client just connected!");

        char clientName[INET_ADDRSTRLEN]; // String to contain client address
        if (inet_ntop(AF_INET, &clientAddress.sin_addr.s_addr, clientName, sizeof(clientName)) != NULL)
        {
            printf("DEBUG: Handling client %s/%d\n", clientName, ntohs(clientAddress.sin_port));
        }
        else
        {
            puts("ERROR: Unable to get client address");
        }

        handleClient(serverSocket, clientSocket);
    }
}

void handleIpv6(int serverSocket)
{
    while (1)
    {
        struct sockaddr_in6 clientAddress;

        socklen_t clientAddressLength = sizeof(clientAddress);

        // Wait for a client to connect
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (clientSocket < 0)
        {
            printErrorAndExit("ERROR: Failed to accept connection from client.");
        }

        puts("INFO: A client just connected!");

        char clientName[INET6_ADDRSTRLEN]; // String to contain client address
        if (inet_ntop(AF_INET6, &clientAddress.sin6_addr.s6_addr, clientName, sizeof(clientName)) != NULL)
        {
            printf("DEBUG: Handling client %s/%d\n", clientName, ntohs(clientAddress.sin6_port));
        }
        else
        {
            puts("ERROR: Unable to get client address");
        }

        handleClient(serverSocket, clientSocket);
    }
}

void handleConnections(int serverSocket, int addressIsV6)
{
    if (addressIsV6)
    {
        handleIpv6(serverSocket);
    }
    else
    {
        handleIpv4(serverSocket);
    }
}

int main(int argc, char *argv[])
{
    // validate the number of argumentos
    // address is mandatory, port is optional (it uses the default one if empty)
    if (argc < 2 || argc > 3)
    {
        printErrorAndExit("ERROR: Invalid arguments. To run the server: server <address format> <port (optional)>");
    }

    char *addressType = argv[1]; // first argument -> v4 ou v6
    validateAddressType(addressType);
    int addressIsV6 = (strcmp(addressType, "v6") == 0);

    int port = DEFAULT_PORT;
    if (argc == 3)
    {
        port = atoi(argv[2]); // second argument (optional) -> port
    }

    // to generate a random number later on
    srand(time(NULL));

    puts("DEBUG: Already got everything we needed to create the socket.");
    int serverSocket = createTcpSocket(addressIsV6);

    puts("DEBUG: Socket created! Binding to local address.");
    bindToLocalAddress(serverSocket, port, addressIsV6);

    setTheServerToListen(serverSocket, port);

    handleConnections(serverSocket, addressIsV6);
}