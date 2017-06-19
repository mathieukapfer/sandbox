#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[])
{
  QTcpServer tcpServer;
  char _buf[1024];
  
  // bind & listen
  if(!tcpServer.listen(QHostAddress::Any, 8080)) {
    std::cout << "[Server] listen/bind error"<< std::endl;
    exit(0);
  } 

  for(;;) {
    // accept
    if(tcpServer.waitForNewConnection(3000)) {
      std::cout << "[Server] new connection" << std::endl;
      QTcpSocket *client = tcpServer.nextPendingConnection();

      // wait data
      client->waitForReadyRead();
      client->readLine(_buf,sizeof(_buf));
      std::cout << "[Server] data recv:" << _buf << std::endl;
    } 
  }
  return 0;
}
