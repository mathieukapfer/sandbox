#include <QCoreApplication>
#include "TcpServer.h"

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  TcpServer tcpServer;
  tcpServer.listen();

  return app.exec();
}
