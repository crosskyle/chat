# Author: Kyle Cross
# Date: 10/17/2017
# Description: This program is a server in a chat client program. The program takes a port number as
# a command line argument. The server continually runs on one thread to allow for subsequent clients
# to connect with it. Communication between the server host and client host must alternate. The user
# can enter a command '\quit' to end communication with the current client host.
# Sources: Some syntax is borrowed from class lecture.

from socket import *
import sys


# Validation
# Input: command line arguments.
# This function takes command line arguments as inputs and verifies that they are valid.
# Output: It returns a valid port number.

def validation(numOfArguments, serverPort):

  # Verify that number of arguments provided is one
  if numOfArguments is not 2:
    print 'There must be one port argument.'
    exit(1)

  #Verify that port number is valid
  if serverPort < 1024 or serverPort > 65535:
    print 'Port number must be within range 1024 - 65535.'
    exit(1)

  return serverPort


# Startup
# Description: A Server's socket is created, bound to a port number, and set to listen on that port
# Input: A port number must be provided as input.
# Output: The socket is the output.

def startup(serverPort):

  serverSocket = socket(AF_INET, SOCK_STREAM)
  serverSocket.bind(('', serverPort))
  serverSocket.listen(1)

  return serverSocket

# ReceiveMessage
# Description: A message is received from a client.
# Input: The connection socket.
# Output: Integer indicating successful execution.

def receiveMessage(connectionSocket):

  # Receive message
  receivedMessage = connectionSocket.recv(2024)

  # Terminate program if '\quit' is received
  if receivedMessage == "\quit":
    connectionSocket.close()
    return -1

  print receivedMessage

  return 0

# SendMessage
# Description: A message is sent to a client.
# Input: The connection socket.
# Output: Integer indicating successful execution.

def sendMessage(connectionSocket):

  # Message received from stdin and sent to client
  sentence = raw_input('Host A> ')
  connectionSocket.send('Host A> ' + sentence)

  # Terminate program if '\quit' is received
  if sentence == '\quit':
    return -1

  return 0

# Main
# Description: The program is executed within main. An while loop runs the server continuously
# until a sigint is received. The inner while loop runs until a '\quit' command is received from
# either the client or the server.

def main():

  # Command line args are validated
  serverPort = validation(len(sys.argv), int(sys.argv[1]))

  # The server is bound to a port and listens for incoming connections
  serverSocket = startup(serverPort)

  # Loops until sigint is received.
  while 1:

    print 'Host A is ready to receive at port ' + str(serverPort)

    # Loops until '\quit' command is received.
    while 1:

      # New connection is accepted
      connectionSocket, addr = serverSocket.accept()

      # Message received from client
      if receiveMessage(connectionSocket) is -1:
        break

      # Message sent to client
      if sendMessage(connectionSocket) is -1:
        break

      # Connection is closed
      connectionSocket.close()


# Execute program
main()
