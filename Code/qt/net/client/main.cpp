#include <QCoreApplication>
#include "TcpClient.h"

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  TcpClient tcpClient;
  tcpClient.tcpConnect();
  
  return app.exec();

}
