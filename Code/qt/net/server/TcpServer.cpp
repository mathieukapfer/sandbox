#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <iostream>
#include <unistd.h>
#include "TcpServer.h"


TcpServer::TcpServer(): _tcpServer(new QTcpServer(this)) {
  memset(_buf, 0, sizeof(_buf));
  connect(_tcpServer, &QTcpServer::newConnection, this, &TcpServer::echo);
}

TcpServer::~TcpServer() {
};

void TcpServer::echo() {
    int numRead=0;
    QTcpSocket *client;

    //std::cout << "wait for client" << std::endl;
    //_tcpServer.waitForNewConnection();
    std::cout << "new Client!" << std::endl;
    client =_tcpServer->nextPendingConnection();
    std::cout << "wait for data from client" << std::endl;

    client->waitForReadyRead();
    numRead = client->readLine(_buf,sizeof(_buf)); 
    std::cout << "data read:" << _buf << std::endl;
    client->write("Echo:");
    client->write(_buf);
    client->write("\n");
    client->waitForBytesWritten();

    //if (numRead == 0 && !client->waitForReadyRead())
    //  break;    
  }

void TcpServer::listen() {
  QHostAddress addr(QHostAddress::LocalHost) ;
    
  if(!_tcpServer->listen(addr /*QHostAddress::Any*/, 8080)) {
    std::cout << "listen/bind error"<< std::endl;
    exit(0);
  } else {
    std::cout << "listen/bind ok"<< std::endl;      
  }
  
  std::cout << "listen on port:" << _tcpServer->serverPort()
            << "(IP:" << addr.toString().toStdString() << ")"
            << std::endl;


  //for(;;) {    
  //}

}
  
bool TcpServer::isListening() {
    return _tcpServer?_tcpServer->isListening():false;
}
