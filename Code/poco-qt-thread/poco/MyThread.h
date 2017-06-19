
#include <Poco/Thread.h>

class MyThread : public Poco::Runnable {
 public:
  explicit MyThread(int num);
  virtual ~MyThread();

  void run();
  void start();
  void wait();

 private:
  Poco::Thread _thread;
  int _num;
};


