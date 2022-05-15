#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// constants related to the protocol defined
static const int BUFSIZE = 500;             // tamanho máximo da mensagem
static const int MAX_PENDING = 5;           // número máximo de solicitações de conexão
static const int DEFAULT_PORT = 51511;      // porta default de conexão
static const int ALLOW_IPV6_CONNECTION = 1; // flag de permissão para aceitar trabalhar com endereços ipv6

// constants related to the commands processed by the server
static const char *KILL_COMMAND = "kill";
static const int KILL = 1;
static const char *ADD_COMMAND = "add";
static const int ADD = 2;
static const char *REMOVE_COMMAND = "remove";
static const int REMOVE = 3;
static const char *LIST_COMMAND = "list";
static const int LIST = 4;
static const char *READ_COMMAND = "read";
static const int READ = 5;

void printErrorAndExit(char *errorMessage)
{
    puts(errorMessage);
    exit(1);
}

void validateAddressType(char *addressType)
{
    if (strcmp(addressType, "v4") != 0 && strcmp(addressType, "v6") != 0)
    {
        printErrorAndExit("Invalid address type. It should be either v4 or v6.");
    }
    if (strcmp(addressType, "v6") != 0 && !ALLOW_IPV6_CONNECTION)
    {
        printErrorAndExit("ipv6 is not supported.");
    }
}

int createIpv4TcpSocket()
{
    int serverSocket;
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printErrorAndExit("Failed to create ipv4 socket.");
    }

    return serverSocket;
}

int createIpv6TcpSocket()
{
    int serverSocket;
    if ((serverSocket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        printErrorAndExit("Failed to create ipv6 socket.");
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
    // cria struct de endereço local
    struct sockaddr_in serverAddress;                  // endereço local
    memset(&serverAddress, 0, sizeof(serverAddress));  // clear out structure
    serverAddress.sin_family = AF_INET;                // IPv4
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // any incoming interface
    serverAddress.sin_port = htons(port);              // local port

    // ligar ao endereço local
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printErrorAndExit("Bind failed!");
        close(serverSocket);
    }
}

void bindToLocalIpv6Address(int serverSocket, int port)
{
    // cria struct de endereço local
    struct sockaddr_in6 serverAddress;                // endereço local
    memset(&serverAddress, 0, sizeof(serverAddress)); // zero out structure
    serverAddress.sin6_family = AF_INET6;             // IPv6
    serverAddress.sin6_addr = in6addr_any;            // any incoming interface
    serverAddress.sin6_port = htons(port);            // local port

    // ligar ao endereço local
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printErrorAndExit("Bind failed!");
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
    printf("Trying to set the server to listen at port %i.\n", port);

    if (listen(serverSocket, MAX_PENDING) < 0)
    {
        printErrorAndExit("Server could not listen.");
        close(serverSocket);
    }

    printf("Server listening at port %i\n", port);
}

int getCommandType(char *command)
{
    int wordLength = strlen(command);

    if (strcmp(command, KILL_COMMAND) == 0)
    {
        return KILL;
    }
    else if (strcmp(command, ADD_COMMAND) == 0)
    {
        return ADD;
    }
    else if (strcmp(command, LIST_COMMAND) == 0)
    {
        return LIST;
    }
    else if (strcmp(command, REMOVE_COMMAND) == 0)
    {
        return REMOVE;
    }
    else if (strcmp(command, READ_COMMAND) == 0)
    {
        return READ;
    }
    else
    {
        return -1;
    }
}

char *processMessage(char *originalMessage, int clientSocket)
{
    char message[BUFSIZE];
    strncpy(message, originalMessage, strlen(originalMessage) - 1);

    // split message by spaces
    char *word;
    int wordCounter = 0;
    char *splitter = " ";

    word = strtok(message, splitter);
    int commandType = getCommandType(word);

    if (commandType < 0)
    {
        close(clientSocket);
        puts("Command unknown sent by client. Connection closed.");
        return NULL;
    }

    while (word != NULL)
    {
        wordCounter += 1;
        word = strtok(NULL, " ");
    }

    // reset message
    memset(&message, 0, sizeof(message));

    return "this is a server response";
}

void handleClient(int clientSocket)
{
    char buffer[BUFSIZE]; // buffer for echo string
    // reset buffer to avoid garbage
    memset(&buffer, 0, sizeof(buffer));

    ssize_t numberOfBytesReceived;
    char *responseToClient;

    // Send received string and receive again until end of stream
    do
    {
        // Receive message from client
        numberOfBytesReceived = recv(clientSocket, buffer, BUFSIZE, 0);
        if (numberOfBytesReceived < 0)
        {
            printErrorAndExit("recv() failed");
        }
        else if (numberOfBytesReceived == 0) {
            puts("Client closed the connection!");
            close(clientSocket);
            return;
        }

        printf("< %s", buffer);

        // process message and return the string to send back to the client
        char message[BUFSIZE];
        // copy into a new array to avoid messing something
        // remove last character because it is a line break
        strncpy(message, buffer, strlen(buffer) - 1);

        responseToClient = processMessage(message, clientSocket);
        memset(&message, 0, sizeof(message)); // clean message up

        // if some error happened, leave the handler
        if (responseToClient == NULL)
        {
            return;
        }

        // 0 indicates end of stream
        // Echo message back to client
        ssize_t numberOfBytesSent = send(clientSocket, responseToClient, strlen(responseToClient), 0);
        printf("> ");
        puts(responseToClient);

        if (numberOfBytesSent < 0)
        {
            printErrorAndExit("send() failed");
        }

        // reset buffer
        memset(&buffer, 0, sizeof(buffer));
        // reset response to client
        memset(&responseToClient, 0, sizeof(responseToClient));

    } while (numberOfBytesReceived > 0);

    close(clientSocket); // Close client socket
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
            printErrorAndExit("Failed to accept connection from client.");
        }

        puts("A client just connected!");

        char clientName[INET_ADDRSTRLEN]; // String to contain client address
        if (inet_ntop(AF_INET, &clientAddress.sin_addr.s_addr, clientName, sizeof(clientName)) != NULL)
        {
            printf("Handling client %s/%d\n", clientName, ntohs(clientAddress.sin_port));
        }
        else
        {
            puts("Unable to get client address");
        }

        handleClient(clientSocket);
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
            printErrorAndExit("Failed to accept connection from client.");
        }

        puts("A client just connected!");

        char clientName[INET6_ADDRSTRLEN]; // String to contain client address
        if (inet_ntop(AF_INET6, &clientAddress.sin6_addr.s6_addr, clientName, sizeof(clientName)) != NULL)
        {
            printf("Handling client %s/%d\n", clientName, ntohs(clientAddress.sin6_port));
        }
        else
        {
            puts("Unable to get client address");
        }

        handleClient(clientSocket);
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
    // validando o número de argumentos
    // assumindo o formato do endereço como obrigatório e a porta como opcional
    // porta padrão é 51511
    if (argc < 2 || argc > 3)
    {
        printErrorAndExit("Invalid arguments. To run the server: server <address format> <port (optional)>");
    }

    char *addressType = argv[1]; // primeiro argumento -> v4 ou v6
    validateAddressType(addressType);
    int addressIsV6 = (strcmp(addressType, "v6") == 0);
    printf("Address version is v6? %i\n", addressIsV6);

    int port = DEFAULT_PORT;
    if (argc == 3)
    {
        port = atoi(argv[2]); // segundo argumento (opcional) -> porta
    }

    puts("Already got everything we needed to create the socket.");
    int serverSocket = createTcpSocket(addressIsV6);

    puts("Socket created! Binding to local address.");
    bindToLocalAddress(serverSocket, port, addressIsV6);

    puts("Let's make this server listen to something.");
    puts("calling setTheServerToListen!");
    setTheServerToListen(serverSocket, port);

    handleConnections(serverSocket, addressIsV6);
}