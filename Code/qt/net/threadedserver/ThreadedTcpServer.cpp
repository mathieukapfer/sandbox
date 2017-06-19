#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <iostream>
#include <unistd.h>
#include "ThreadedTcpServer.h"
#include "TcpThread.h"

ThreadedTcpServer::ThreadedTcpServer(): _tcpServer(new QTcpServer(this)) {
  connect(_tcpServer, &QTcpServer::newConnection, this, &ThreadedTcpServer::newConnectionHandler);
}

ThreadedTcpServer::~ThreadedTcpServer() {
};


void ThreadedTcpServer::newConnectionHandler() {

    //std::cout << "wait for client" << std::endl;
    //_tcpServer.waitForNewConnection();
    std::cout << "[server] new Client!" << std::endl;
    QTcpSocket *client =_tcpServer->nextPendingConnection();
    TcpThread *thread = new TcpThread (client);
    // client->moveToThread(thread);
    // connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(client, &QIODevice::readyRead, thread, &TcpThread::echo );
    connect(client, &QTcpSocket::disconnected, thread, &TcpThread::quit );
    thread->start();

    //if (numRead == 0 && !client->waitForReadyRead())
    //  break;    
  }

void ThreadedTcpServer::listen() {
  QHostAddress addr(QHostAddress::LocalHost) ;
    
  if(!_tcpServer->listen(addr /*QHostAddress::Any*/, 8080)) {
    std::cout << "[server] listen/bind error"<< std::endl;
    exit(0);
  } else {
    std::cout << "[server] listen/bind ok"<< std::endl;      
  }
  
  std::cout << "[server] listen on port:" << _tcpServer->serverPort()
            << "(IP:" << addr.toString().toStdString() << ")"
            << std::endl;

}
  
bool ThreadedTcpServer::isListening() {
    return _tcpServer?_tcpServer->isListening():false;
}
