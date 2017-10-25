from socket import *
import sys

if len(sys.argv) is not 2:
  print 'There must be one port argument.'
else:
  try:
    serverPort = int(sys.argv[1])
    if serverPort < 1024 or serverPort > 65535:
      print 'Port number must be within range 1024 - 65535.'
    else:
      handle = 'Fluffykins'

      serverSocket = socket(AF_INET, SOCK_STREAM)
      serverSocket.bind(('', serverPort))
      serverSocket.listen(1)

      while 1:

        print 'The server is ready to receive at port ' + str(serverPort)

        while 1:
          connectionSocket, addr = serverSocket.accept()

          receivedMessage = connectionSocket.recv(2024)

          if receivedMessage == "\quit":
            connectionSocket.close()
            break

          print receivedMessage

          sentence = raw_input(handle + '> ')

          connectionSocket.send(handle + '> ' + sentence + '@@')

          connectionSocket.close()

          if sentence == '\quit':
            break

  except ValueError:
    print 'Port number must be an integer.'



