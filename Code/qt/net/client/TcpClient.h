#include <QtCore/QObject>
#include <QtNetwork/QAbstractSocket>
 
class QTcpSocket;

class TcpClient: public QObject 
{
    Q_OBJECT

public:
  TcpClient();
  virtual ~TcpClient() {};

  void tcpConnect();
  QAbstractSocket::SocketState state();              

private slots:
  void sayHello();
  
private:
  QTcpSocket *_tcpSocket;
  char _buf[1024];

};
