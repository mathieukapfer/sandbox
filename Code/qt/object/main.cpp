#include "Test.h"

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);
  Test test;
  QObject::connect(&test, &Test::closed, &a, &QCoreApplication::quit);

  return a.exec();
}
