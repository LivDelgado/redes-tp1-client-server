#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static const int MAX_PENDING = 5; // número máximo de solicitações de conexão
static const int DEFAULT_PORT = 51511; // porta default de conexão
static const int ALLOW_IPV6_CONNECTION = 1; // flag de permissão para aceitar trabalhar com endereços ipv6

void printErrorAndExit(char *errorMessage) {
    puts(errorMessage);
    exit(1);
}

void validateAddressType(char *addressType) {
    if (strcmp(addressType, "v4") != 0 && strcmp(addressType, "v6") != 0) {
        printErrorAndExit("Invalid address type. It should be either v4 or v6.");
    }
    if (strcmp(addressType, "v6") != 0 && !ALLOW_IPV6_CONNECTION) {
        printErrorAndExit("ipv6 is not supported.");
    }
}

int createIpv4TcpSocket() {
    int serverSocket;
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printErrorAndExit("Failed to create ipv4 socket.");
    }

    return serverSocket;
}

int createIpv6TcpSocket() {
    int serverSocket;
    if ((serverSocket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printErrorAndExit("Failed to create ipv6 socket.");
    }

    return serverSocket;
}

int createTcpSocket(int addressIsV6) {
    if (addressIsV6) {
        return createIpv6TcpSocket();
    } else {
        return createIpv4TcpSocket();
    }
}

void bindToLocalIpv4Address(int serverSocket, int port) {
    // cria struct de endereço local
    struct sockaddr_in serverAddress; // endereço local
    memset(&serverAddress, 0, sizeof(serverAddress)); // zero out structure
    serverAddress.sin_family = AF_INET; // IPv4
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // any incoming interface
    serverAddress.sin_port = htons(port); // local port

    // ligar ao endereço local
    if (bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        printErrorAndExit("Bind failed!");
    }
}

void bindToLocalIpv6Address(int serverSocket, int port) {
    // cria struct de endereço local
    struct sockaddr_in6 serverAddress; // endereço local
    memset(&serverAddress, 0, sizeof(serverAddress)); // zero out structure
    serverAddress.sin6_family = AF_INET6; // IPv6
    serverAddress.sin6_addr = in6addr_any; // any incoming interface
    serverAddress.sin6_port = htons(port); // local port

    // ligar ao endereço local
    if (bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        printErrorAndExit("Bind failed!");
    }
}

void bindToLocalAddress(int serverSocket, int port, int addressIsV6) {
    if (addressIsV6) {
        bindToLocalIpv6Address(serverSocket, port);
    } else {
        bindToLocalIpv4Address(serverSocket, port);
    }
}

void setTheServerToListen(int serverSocket, int port) {
    if (listen(serverSocket, MAX_PENDING) < 0) {
        printErrorAndExit("Server could not listen.");
    }

    fprintf(stdout, "Server listening at port %i", port);
}

void handleConnections(int serverSocket) {
    
}

int main(int argc, char *argv[]) {
    // validando o número de argumentos
    // assumindo o formato do endereço como obrigatório e a porta como opcional
    // porta padrão é 51511
    if (argc < 2 || argc > 3) {
        printErrorAndExit("Invalid arguments. To run the server: server <address fornat> <port (optional)>");
    }

    char *addressType = argv[1]; // primeiro argumento -> v4 ou v6
    validateAddressType(addressType);
    int addressIsV6 = (addressType == "v6");

    int port = DEFAULT_PORT;
    if (argc == 3) {
        port = atoi(argv[2]); // segundo argumento (opcional) -> porta
    }

    int serverSocket = createTcpSocket(addressIsV6);
    bindToLocalAddress(serverSocket, port, addressIsV6);
    setTheServerToListen(serverSocket, port);

    handleConnections(serverSocket);
}