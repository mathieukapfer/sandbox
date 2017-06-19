#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QtCore/QObject>

class QTcpServer;

class TcpServer: public QObject {

  Q_OBJECT

public:
  TcpServer(); 
  virtual ~TcpServer();
  void listen();
  bool isListening();
                     
private slots:
  void echo();
  
private:
  QTcpServer *_tcpServer;
  char _buf[1024];
};

#endif /* TCPSERVER_H */
