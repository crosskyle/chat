
from socket import *

serverName = 'localhost'
serverPort = 65535
handle = 'HostB'

while 1:
  clientSocket = socket(AF_INET, SOCK_STREAM)
  clientSocket.connect((serverName, serverPort))

  sentence = raw_input(handle + '> ')

  if sentence == '\quit':
    clientSocket.send('\quit')
    clientSocket.close()
    break

  message = handle + '> ' + sentence

  clientSocket.send(message)

  receivedSentence = clientSocket.recv(1024)

  arr = receivedSentence.split(' ')

  if arr[1] == '\quit':
    clientSocket.send('\quit')
    clientSocket.close()
    break

  print receivedSentence

  clientSocket.close()


