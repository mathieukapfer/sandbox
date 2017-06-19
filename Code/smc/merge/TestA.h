#ifndef TESTA_H
#define TESTA_H

#include <stdarg.h>
#include "Test.h"

#define TRACE(...) printf(__VA_ARGS__)
#include "testA_sm.h"

namespace ProdA { 

class TestA : public Test {
 public:
  TestA();
  virtual ~TestA();

  // redefined services
  void sayEntry(const char * who);
  void sayExit(const char * who);

  // state machine and associated variable
  testAContext _fsm;

};

} // namespace prodA
#endif /* TESTA_H */
