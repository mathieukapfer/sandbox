#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QTcpSocket>
#include <iostream>
#include <unistd.h>
#include "TcpClient.h"

TcpClient::TcpClient(): _tcpSocket(new QTcpSocket(this))  {
  //connect(_tcpSocket, &QIODevice::readyRead, this, &TcpClient::sayHello);
  connect(_tcpSocket, &QAbstractSocket::connected, this, &TcpClient::sayHello);
}

QAbstractSocket::SocketState TcpClient::state(){
  return _tcpSocket->state();
}             


void TcpClient::tcpConnect(){

  std::cout << "try connection ..." << std::endl;
  _tcpSocket->connectToHost("127.0.0.1", 8080);
  
  if (!_tcpSocket->waitForConnected()) {
    std::cout <<_tcpSocket->errorString().toStdString() << std::endl;
    exit(0);
  } else {
    std::cout << "[client] connection done" << std::endl;
  }
  //_tcpSocket->waitForDisconnected();
}

void TcpClient::sayHello() {

  //_tcpSocket->waitForConnected();
  std::cout << "[client] client connected!" << std::endl;
  _tcpSocket->write("Hello\n");
  std::cout << "[client] say hello ..." << std::endl;
  _tcpSocket->waitForBytesWritten();
  std::cout << "[client] Waiting echo ..." << std::endl;
  _tcpSocket->waitForReadyRead();
  _tcpSocket->readLine(_buf,sizeof(_buf));
  std::cout << "[client] data read:" << _buf << std::endl;
  
}
  
