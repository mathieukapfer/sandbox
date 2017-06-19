#include <QtNetwork/QTcpSocket>
#include <iostream>
#include <unistd.h>

void sayHello(QTcpSocket &tcpSocket) {
  std::cout << "Say hello " << std::endl;
  tcpSocket.write("Hello\n");
  tcpSocket.waitForBytesWritten();  
}

int main(int argc, char *argv[])
{
  QTcpSocket tcpSocket;
  
  std::cout << "try connection ..." << std::endl;
  tcpSocket.connectToHost("127.0.0.1", 8080);

  if (!tcpSocket.waitForConnected()) {
    std::cout <<tcpSocket.errorString().toStdString() << std::endl;
    exit(0);
  } else {
    std::cout << "connection done" << std::endl;
  }

  sayHello(tcpSocket);
  
  return 0;
}
