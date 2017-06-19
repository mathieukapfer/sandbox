#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QtCore/QObject>

class QTcpServer;

class ThreadedTcpServer: public QObject {

  Q_OBJECT

public:
  ThreadedTcpServer(); 
  virtual ~ThreadedTcpServer();
  void listen();

  bool isListening();

private slots:
  void newConnectionHandler();
  
private:
  QTcpServer *_tcpServer;
};

#endif /* TCPSERVER_H */
