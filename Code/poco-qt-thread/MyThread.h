
#ifdef ENABLE_QT
#include <QtCore/QThread>
#else
#include <Poco/Thread.h>
#endif

class MyThread :
#ifdef ENABLE_QT
  public QThread
#else
  public Poco::Runnable
#endif
{
 public:
  explicit MyThread(int num);
  virtual ~MyThread();

  void run();
#ifndef ENABLE_QT
  void start();
  void wait();
#endif

 private:
#ifndef ENABLE_QT
  Poco::Thread _thread;
#endif
  int _num;
};


