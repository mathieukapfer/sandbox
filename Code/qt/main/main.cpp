#include <QtCore>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);
  QCommandLineParser parser;
  parser.addHelpOption();

#if 0
  // A boolean option with a single name (-p)
  QCommandLineOption showProgressOption("p", QCoreApplication::translate("main", "Show progress during copy"));
  parser.addOption(showProgressOption);

  // A boolean option with multiple names (-f, --force)
  QCommandLineOption forceOption
    (QStringList() << "f" << "force",
     QCoreApplication::translate("main", "Overwrite existing files."));
  parser.addOption(forceOption);

  // An option with a value
  QCommandLineOption targetDirectoryOption
    (QStringList() << "t" << "target-directory",
     QCoreApplication::translate("main", "Copy all source files into <directory>."),
     QCoreApplication::translate("main", "directory"));
  parser.addOption(targetDirectoryOption);
#elif 0
  parser.addOptions(
    {
      // A boolean option with a single name (-p)
      {"p", QCoreApplication::translate("main", "Show progress during copy") },
      // A boolean option with multiple names (-f, --force)
      { {"f", "force"}, QCoreApplication::translate("main", "Overwrite existing files.") },
      // An option with a value
      { {"t", "target-directory"} ,
          QCoreApplication::translate("main", "Copy all source files into <directory>."),
          QCoreApplication::translate("main", "directory") }
    });
#elif 1
  parser.addOptions(
    {
      // A boolean option with a single name (-p)
      {"p", "Show progress during copy" },
      // A boolean option with multiple names (-f, --force)
      { {"f", "force"}, "Overwrite existing files XXX." },
      // An option with a value
      { {"t", "target-directory"} , "Copy all source files into <directory>.", "directory" }
    });
#else
  parser.addOptions(
    {
      { {"soap", "s"}, "Start Borne in SOAP mode" },
      { {"ws", "w"}, "Start Borne in WS mode" },
      { {"debug", "d"}, "Start Borne with debug logs enable" },
      { "newmotion", "Connect Charging Station to Newmotion (SOAP or WS)" },
      { "eurisko", "Connect Charging Station to Eurisko (192.168.0.156)" },
        { "version", "Choose Ocpp Version (1.5 by default)", "version", "1.5" },
      { "ipsupervision",  "Connect to given Supervision (ip)", "ip" },
      { "ipserversoap",  "Soap server: listen to request on given (local) ip", "ip"},
    });
#endif

  // Process the actual command line arguments given by the user
  parser.process(app);

  // Option call back
  //bool force = parser.isSet(forceOption);
  bool force = parser.isSet("force");
  QString targetDir = parser.value("t");

  cout << (force?"force ON":"force off") << endl;
  cout << "targetDir:" << targetDir.toStdString()  << endl;
  cout << endl;
}
