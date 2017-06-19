#ifndef TESTB_H
#define TESTB_H

#include <stdarg.h>
#include "Test.h"

#define TRACE(...) printf(__VA_ARGS__)
#include "testB_sm.h"

namespace ProdB {

class TestB : public Test {
 public:
  TestB();
  virtual ~TestB();

  // redefined services
  void sayEntry(const char * who);
  void sayExit(const char * who);

  // state machine and associated variable
  testBContext _fsm;

};
}// namespace ProdB
#endif /* TESTA_H */
