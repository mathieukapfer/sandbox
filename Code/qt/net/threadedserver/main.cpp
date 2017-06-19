#include <QCoreApplication>
#include "ThreadedTcpServer.h"

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  ThreadedTcpServer tcpServer;
  tcpServer.listen();

  return app.exec();
}
