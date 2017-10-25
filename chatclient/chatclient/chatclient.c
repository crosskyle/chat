// Author: Kyle Cross
// Title: chatclient.c
// Date: 10/25/17
// Description: This program is a client in chat client program where there is alternating communication
// between this program and a server program. The client takes a hostname and port number as command
// line arguments. The client creates a new connection for each time it contacts the server. Communication
// between the server host and client host must alternate. The user can enter the '\quit' command to end
// communication with the server and end the client program execution.
// Sources: Some syntax and comments are borrowed from Operating Systems I lectures. Especially setting up
// the server address struct.
// Testing: Tested on flip1.engr.oregonstate.edu

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


// Validation
// Input: Command line arguments
// Description: Command line arguments are validated as a good hostname and port number.
// Output: An integer indicating valid arguments.

int validation(int argc, char *argv[]) {
    
    // Verify the number of args is two
    if (argc != 3) {
        printf("%s\n", "Please provide a hostname and port number.");
        return -1;
    }
    
    //Verify the port number is valid
    if (atoi(argv[2]) < 1024 || atoi(argv[2]) > 65535) {
        printf("%s\n", "Please provide a port number in range 1024 - 65535.");
        return -1;
    }
    
    return 0;
}


// CreateServerAddressStruct
// Description: a struct is created that holds a valid server address of the chat server.
// Input: A port number and hostname of the chat server.
// Output: A server address struct

struct sockaddr_in createServerAddressStruct(int portNumber, char* hostName) {
    struct hostent* serverHostInfo;
    struct sockaddr_in serverAddress;
    
    // Set up the server address struct
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    serverAddress.sin_family = AF_INET; // Create a socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverHostInfo = gethostbyname(hostName); // Convert the machine name into a special form of address
    if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address
    
    return serverAddress;
}


// InitiateContact
// Description: Initate contact with the server by settup of socket and connecting to server
// Input: Port number and server address struct
// Output: Socket file description

int initiateContact(int portNumber, struct sockaddr_in serverAddress) {
    
    // Set up the socket
    int socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0) {
        fprintf(stderr, "Error opening socket\n"); exit(1);
    }
    
    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) { // Connect socket to address
        fprintf(stderr, "Error: could not contact chatserve on port %d", portNumber); exit(2);
    }
    
    return socketFD;
}


// SendMessage
// Description: A message is sent to the server.
// Input: The client's handle and socket file description
// Output: An integer indication of successful execution

int sendMessage(char* handle, int socketFD) {
    printf("%s", handle);
    
    //Using fgets to get a maximum of 2048 characters from stdin
    char input[2048];
    memset(input, '\0', 2048);
    fgets(input, 2048, stdin);
    int userInputLength = (int)strlen(input);
    input[userInputLength-1] = '\0';
    
    // Terminate program if '\quit' command received and let server know
    if (strcmp(input, "\\quit") == 0) {
        send(socketFD, input, 5, 0);
        close(socketFD);
        return -1;
    }
    
    // Message sent is composed of handle and user input
    char message[2048];
    memset(message, '\0', 2048);
    strcat(message, handle);
    strcat(message, input);
    int messageLength = (int)strlen(message);
    
    // Send message to server
    send(socketFD, message, messageLength, 0);
    
    return 0;
}


// ReceiveMessage
// Description: Message is received from Server
// Input: Socket file description
// Output: An integer indication of successful execution

int receiveMessage(int socketFD) {
    // Get message from server
    char buffer[2048];
    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer
    recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
    
    // Exit program if '\quit' command is received
    if (strcmp((strstr(buffer, "> ")), "> \\quit") == 0) {
        close(socketFD);
        return -1;
    }
    
    printf("%s\n", buffer);
    
    return 0;
}

// GetHandle
// Description: User's handle obtained through stdin
// Input: Handle String

void getHandle(char* handle) {
    
    printf("%s", "Enter a handle: ");
    
    //Using fgets to get a handle from stdin
    memset(handle, '\0', 2048);
    fgets(handle, 2048, stdin);
    int handleLength = (int)strlen(handle);
    handle[handleLength-1] = '\0';
    strcat(handle, "> ");
}


// Main
// Description: The program is executed within main. A while loop loops until
// a '\quit' command is received from the client or server, and the program stops
// execution after.
// Input: Command line arguments hostname and port number.
// Ouput: Exit status code

int main(int argc, char *argv[]) {
    
    // Command line arguments validated
    if (validation(argc, argv) == -1) {
        return 1;
    }
    
    int socketFD;   // socket file description
    int portNumber = atoi(argv[2]); // port number of listening server
    char* hostName = argv[1]; // name of server
    struct sockaddr_in serverAddress; // server address struct
    
    // server address struct is created
    serverAddress = createServerAddressStruct(portNumber, hostName);
    
    // hand is obtained from stdin input
    char handle[2048];
    getHandle(handle);
    
    // Loops until '\quit' received
    while (1) {
        socketFD = initiateContact(portNumber, serverAddress);
        
        // Send mesage to server
        if (sendMessage(handle, socketFD) == -1) {
            break;
        }
        
        // Receive message from server
        if (receiveMessage(socketFD) == -1) {
            break;
        }
        
        close(socketFD); // Close the socket
    }
 
    return 0;
}


