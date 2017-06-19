#ifndef TCPTHREAD_H
#define TCPTHREAD_H

#include <QThread>

class QTcpSocket;

class TcpThread : public QThread
{
    Q_OBJECT

public:
  TcpThread(QTcpSocket *client);
  virtual ~TcpThread();

public slots:
  void echo();

private:
  QTcpSocket *m_client;
  char _buf[1024];
};


#endif /* TCPTHREAD_H */
