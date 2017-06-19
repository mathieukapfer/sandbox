#include "TestA.h"
#include <stdio.h>

namespace ProdA { 

TestA::TestA():
  _fsm(*this) {
}

TestA::~TestA() {
}

void TestA::sayEntry(const char * who) {
  Test::say("[wrapped by TestA] ", false);
  Test::sayEntry(who);
}

void TestA::sayExit(const char * who) {
  Test::say("[wrapped by TestA] ", false);
  Test::sayExit(who);
}

}
