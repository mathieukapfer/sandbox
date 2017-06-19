#include <QtCore/QObject>
#include <QtCore/QTimerEvent>
#include "log.h"

class Test : public QObject {
  Q_OBJECT
 public:
  Test() {
    LOG_ENTRY();
    startTimer(1000);
  };

  virtual ~Test() {};

 protected:
  void timerEvent(QTimerEvent *event) {
    LOG_ENTRY();
    (void) event;
    LOG_INFO("emit signal 'close'")
    emit(closed());
  }

Q_SIGNALS:
  void closed();

};

