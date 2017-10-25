#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;
    
    char buffer[200000];
    memset(buffer, '\0', sizeof(buffer));
    
    // Set up the server address struct
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    portNumber = 65535; // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET; // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
    if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address
    
    
    printf("%s\n", "Enter a handle: ");
    
    //Using fgets to get a maximum of 2048 characters from stdin
    char handle[2048];
    memset(handle, '\0', 2048);
    fgets(handle, 2048, stdin);
    int handleLength = (int)strlen(handle);
    handle[handleLength-1] = '\0';
    handle[handleLength+1] = '\0';
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
            charsWritten = send(socketFD, input, userInputLength, 0);
            close(socketFD);
            break;
        }
        
        char message[2048];
        memset(message, '\0', 2048);
        strcat(message, handle);
        strcat(message, input);
        int messageLength = (int)strlen(message);
        
        charsWritten = send(socketFD, message, messageLength, (int)0);
        
        // Get message from server
        memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse
        char readBuffer[1010];
        
        // The readBuffer continually reads messages sent from server and concatenates it onto the full message received in the
        // buffer variable until the @@ chars are received which mark the end of the sent message
        while (strstr(buffer, "@@") == NULL) {
            memset(readBuffer, '\0', sizeof(readBuffer));
            charsRead = recv(socketFD, readBuffer, sizeof(readBuffer) - 1, 0); // Read data from the socket, leaving \0 at end
            strcat(buffer, readBuffer);
            if (charsRead < 0) error("CLIENT: ERROR reading from socket");
        }
        
        // The @@ chars are removed from the decrypted message and the message is sent to stdout
        int terminalLocation = strstr(buffer, "@@") - buffer;
        buffer[terminalLocation] = '\0';
        printf("%s\n", buffer);
        
        close(socketFD); // Close the socket
    }
 
    return 0;
}


