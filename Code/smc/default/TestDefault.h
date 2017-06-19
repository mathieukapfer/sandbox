#ifndef TESTDEFAULT_H
#define TESTDEFAULT_H

#include <stdarg.h>

#define TRACE(...) printf(__VA_ARGS__)
#include "testDefault_sm.h"

class TestDefault {
 public:
  TestDefault();
  virtual ~TestDefault();

  void say(const char * something, bool newline = true);
  const char *getState();
  bool displayState();

  testDefaultContext _fsm;
};


#endif /* TESTDEFAULT_H */
