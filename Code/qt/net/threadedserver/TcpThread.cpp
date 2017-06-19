#include "TcpThread.h"
#include <QTcpSocket>
#include <iostream>

TcpThread::TcpThread(QTcpSocket *client):m_client(client) {
  memset(_buf, 0, sizeof(_buf));  
}

TcpThread::~TcpThread() {};

void TcpThread::echo() {

  // m_client->waitForReadyRead(); => block the thread !!! (the object is not own by this thread)
  m_client->readLine(_buf,sizeof(_buf)); 
  std::cout << "[server thread] data read:" << _buf << std::endl;
  m_client->write("Echo:");
  m_client->write(_buf);
  m_client->write("\n");
  m_client->waitForBytesWritten();

  // release connection
  m_client->disconnectFromHost();
  //m_client->waitForDisconnected();
}

