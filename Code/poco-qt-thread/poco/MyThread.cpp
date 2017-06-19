#include <unistd.h>
#include <iostream>
#include "MyThread.h"

using namespace std;


MyThread::MyThread(int num):
  _num(num) {
}

MyThread::~MyThread() {
}

void MyThread::run() {
  int count = 0;

  do {
    cout << "Thread" << _num << ": Hello world" << endl;
    sleep(1);
  }  while (count++ < 1);
  cout << "Thread" << _num << ": quit" << endl;
}

void MyThread::start() {
  _thread.start(*this);
}

void MyThread::wait() {
  _thread.join();
}

int main(int argc, char *argv[]) {
  MyThread myThread1(1);
  MyThread myThread2(2);

  cout << "Main: launch threads" << endl;
  myThread1.start();
  myThread2.start();
  cout << "Main: join threads" << endl;
  myThread1.wait();
  myThread2.wait();
  cout << "Main: quit" << endl;

  return 0;
}
