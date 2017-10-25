#
#
#
#


from socket import *
import sys


def validation(numOfArguments, serverPort):

  if numOfArguments is not 2:
    print 'There must be one port argument.'
    exit(1)

  if serverPort < 1024 or serverPort > 65535:
    print 'Port number must be within range 1024 - 65535.'
    exit(1)

  return serverPort



def startup(serverPort):

  serverSocket = socket(AF_INET, SOCK_STREAM)
  serverSocket.bind(('', serverPort))
  serverSocket.listen(1)

  return serverSocket



def receiveMessage(connectionSocket):

  receivedMessage = connectionSocket.recv(2024)

  if receivedMessage == "\quit":
    connectionSocket.close()
    return -1

  print receivedMessage

  return 0



def sendMessage(connectionSocket):

  sentence = raw_input('Host A> ')
  connectionSocket.send('Host A> ' + sentence)

  if sentence == '\quit':
    return -1

  return 0



def main():

  serverPort = validation(len(sys.argv), int(sys.argv[1]))

  serverSocket = startup(serverPort)

  while 1:

    print 'Host A is ready to receive at port ' + str(serverPort)

    while 1:

      connectionSocket, addr = serverSocket.accept()

      if receiveMessage(connectionSocket) is -1:
        break

      if sendMessage(connectionSocket) is -1:
        break

      connectionSocket.close()


main()

