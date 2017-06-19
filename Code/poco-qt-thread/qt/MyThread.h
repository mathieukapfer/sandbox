
#include <QtCore/QThread>

class MyThread : public QThread {
 public:
  explicit MyThread(int num);
  virtual ~MyThread();

  void run();

 private:
  int _num;
};


