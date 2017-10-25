// Author: Kyle Cross
// Date: 10/25/17
// Description:
//
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


int validation(int argc, char *argv[]) {
    
    if (argc != 3) {
        printf("%s\n", "Please provide a hostname and port number.");
        return -1;
    }
    
    if (atoi(argv[2]) < 1024 || atoi(argv[2]) > 65535) {
        printf("%s\n", "Please provide a port number in range 1024 - 65535.");
        return -1;
    }
    
    return 0;
}

void initiateContact(int socketFD, int portNumber, struct sockaddr_in serverAddress) {
    
    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) { // Connect socket to address
        fprintf(stderr, "Error: could not contact chatserve on port %d", portNumber); exit(2);
    }
}

int sendMessage(char* handle, int socketFD) {
    printf("%s", handle);
    
    //Using fgets to get a maximum of 2048 characters from stdin
    char input[2048];
    memset(input, '\0', 2048);
    fgets(input, 2048, stdin);
    int userInputLength = (int)strlen(input);
    input[userInputLength-1] = '\0';
    
    if (strcmp(input, "\\quit") == 0) {
        send(socketFD, input, 5, 0);
        close(socketFD);
        return -1;
    }
    
    char message[2048];
    memset(message, '\0', 2048);
    strcat(message, handle);
    strcat(message, input);
    int messageLength = (int)strlen(message);
    
    send(socketFD, message, messageLength, 0);
    
    return 0;
}

int receiveMessage(int socketFD) {
    // Get message from server
    char buffer[2048];
    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
    recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
    
    printf("%s\n", buffer);
    
    // Exit program if '\quit' command is received
    if (strcmp((strstr(buffer, "> ")), "> \\quit") == 0) {
        close(socketFD);
        return -1;
    }
    
    return 0;
}

struct sockaddr_in createServerAddressStruct(int portNumber, char* hostName) {
    struct hostent* serverHostInfo;
    struct sockaddr_in serverAddress;
    
    // Set up the server address struct
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    serverAddress.sin_family = AF_INET; // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber); // Store the port number in server address struct
    serverHostInfo = gethostbyname(hostName); // Convert the machine name into a special form of address
    if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address
    
    return serverAddress;
}

void getHandle(char* handle) {
    
    printf("%s", "Enter a handle: ");
    
    //Using fgets to get a maximum of 2048 characters from stdin
    memset(handle, '\0', 2048);
    fgets(handle, 2048, stdin);
    int handleLength = (int)strlen(handle);
    handle[handleLength-1] = '\0';
    strcat(handle, "> ");
}


int main(int argc, char *argv[]) {
    
    if (validation(argc, argv) == -1) {
        return 1;
    }
    
    int socketFD;
    int portNumber = atoi(argv[2]);
    char* hostName = argv[1];
    struct sockaddr_in serverAddress;
    
    serverAddress = createServerAddressStruct(portNumber, hostName);
    
    char handle[2048];
    getHandle(handle);
    
    while (1) {
        // Set up the socket
        socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
        if (socketFD < 0) {
            fprintf(stderr, "Erro opening socket\n"); exit(1);
        }
        
        initiateContact(socketFD, portNumber, serverAddress);
        
        if (sendMessage(handle, socketFD) == -1) {
            break;
        }
        
        if (receiveMessage(socketFD) == -1) {
            break;
        }
        
        close(socketFD); // Close the socket
    }
 
    return 0;
}


