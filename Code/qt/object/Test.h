#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtCore/QTimerEvent>

class Test : public QObject {
  Q_OBJECT
 public:
  Test() {
    startTimer(1000);
  };

  virtual ~Test() {}

 protected:
  void timerEvent(QTimerEvent *event) {
    (void) event;
    emit(closed());
  }

Q_SIGNALS:
  void closed();

};

