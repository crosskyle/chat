#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) { perror(msg); exit(0); } // Prints errors



int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("%s\n", "Please provide a hostname and port number.");
        return 1;
    }
    
    if (atoi(argv[2]) < 1024 || atoi(argv[2]) > 65535) {
        printf("%s\n", "Please provide a port number in range 1024 - 65535.");
        return 1;
    }
    
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;
    
    char buffer[200000];
    memset(buffer, '\0', sizeof(buffer));
    
    // Set up the server address struct
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET; // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverHostInfo = gethostbyname(argv[1]); // Convert the machine name into a special form of address
    if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address
    
    
    printf("%s", "Enter a handle: ");
    
    //Using fgets to get a maximum of 2048 characters from stdin
    char handle[2048];
    memset(handle, '\0', 2048);
    fgets(handle, 2048, stdin);
    int handleLength = (int)strlen(handle);
    handle[handleLength-1] = '\0';
    strcat(handle, "> ");
    
    
    while (1) {
        // Set up the socket
        socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
        if (socketFD < 0) error("CLIENT: ERROR opening socket");
        
        // Connect to server
        if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) { // Connect socket to address
            fprintf(stderr, "Error: could not contact chatserve on port %d", portNumber); exit(2);
        }
        
        printf("%s", handle);
        
        //Using fgets to get a maximum of 2048 characters from stdin
        char input[2048];
        memset(input, '\0', 2048);
        fgets(input, 2048, stdin);
        int userInputLength = (int)strlen(input);
        input[userInputLength-1] = '\0';
        
        if (strcmp(input, "\\quit") == 0) {
            charsWritten = send(socketFD, input, 5, 0);
            close(socketFD);
            break;
        }
        
        char message[2048];
        memset(message, '\0', 2048);
        strcat(message, handle);
        strcat(message, input);
        int messageLength = (int)strlen(message);
        
        charsWritten = send(socketFD, message, messageLength, 0);
        
        // Get message from server
        memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
        charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
        if (charsRead < 0) error("CLIENT: ERROR reading from socket");
        
        printf("%s\n", buffer);
        
        // Exit program if '\quit' command is received
        if (strcmp((strstr(buffer, "> ")), "> \\quit") == 0) {
            close(socketFD);
            break;
        }
        
        close(socketFD); // Close the socket
    }
 
    return 0;
}


