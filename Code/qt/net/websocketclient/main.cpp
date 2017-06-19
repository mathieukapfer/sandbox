#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCommandLineOption>
#include "WebsocketClient.h"
#include <unistd.h>

#if 0
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("QtWebSockets example: echoclient");
    parser.addHelpOption();

    QCommandLineOption dbgOption(QStringList() << "d" << "debug",
            QCoreApplication::translate("main", "Debug output [default: off]."));
    parser.addOption(dbgOption);
    parser.process(a);
    bool debug = parser.isSet(dbgOption);

    EchoClient client(QUrl(QStringLiteral("ws://localhost:1234")), debug);
    QObject::connect(&client, &EchoClient::closed, &a, &QCoreApplication::quit);

    return a.exec();
}


#else
int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  
  EchoClient client(QUrl(QStringLiteral("ws://localhost:1234")), true);
  QObject::connect(&client, &EchoClient::closed, &a, &QCoreApplication::quit);

  return a.exec();
  //qt_select_msecs(0,0,0,1000);
  ///while(1) {sleep(1);}
}
#endif
