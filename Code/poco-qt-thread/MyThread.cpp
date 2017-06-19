#include <unistd.h>
#include <iostream>
#include "MyThread.h"

using namespace std;

#ifdef ENABLE_QT
#include <QtCore/QSharedPointer>
#else
#include <Poco/SharedPtr.h>
#endif

#ifdef ENABLE_QT
QSharedPointer<int> globalSharedPtr;
#else
Poco::SharedPtr<int> globalSharedPtr;
#endif

void logSharedPtr(int num) {
  if (globalSharedPtr.isNull()) {
    cout << "Global: shared ptr is NULL" << endl;
  } else {
    cout << num << ":" << *globalSharedPtr << " ";
  }
}


MyThread::MyThread(int num):
  _num(num) {
}

MyThread::~MyThread() {
}

void MyThread::run() {
  int count = 0;
  do {
  try {
    cout << "Thread" << _num << ": Hello world"  << endl;
    while (
#ifdef ENABLE_QT
      // mandatory with Qt as no excpetion raised
      !globalSharedPtr.isNull() &&
#endif
      (*globalSharedPtr)++ < 100) {logSharedPtr(_num);};
    sleep(2);
  }
  // use only by Poco to managed NULL shared pointer
  catch(std::exception &e) {
    cout << "Thread" << _num << "exception" << e.what() << endl;
    sleep(2);
  }
  } while (count++ < 1);
  cout << "Thread" << _num << ": quit" << endl;
}

#ifndef ENABLE_QT
void MyThread::start() {
  _thread.start(*this);
}

void MyThread::wait() {
  _thread.join();
}
#endif


int main(int argc, char *argv[]) {
  MyThread myThread1(1);
  MyThread myThread2(2);

  cout << "Main: launch threads" << endl;
  myThread1.start();
  myThread2.start();
  sleep(1);
  cout << "Main: init shared ptr" << endl;

  // play with shared pointer
  logSharedPtr(0);

#ifdef ENABLE_QT
  globalSharedPtr = QSharedPointer<int>(new int);
#else
  globalSharedPtr.assign(new int);
#endif
  cout << "Main: join threads" << endl;
  myThread1.wait();
  myThread2.wait();
  cout << "Main: quit" << endl;

  return 0;
}
