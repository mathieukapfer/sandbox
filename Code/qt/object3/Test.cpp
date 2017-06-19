#include <QtCore/QCoreApplication>
#include "Test.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);
  Test test;
  LOG_ENTRY();
  QObject::connect(&test, &Test::closed, &a, &QCoreApplication::quit);
  LOG_INFO("start loop");
  return a.exec();
  LOG_EXIT();
}
