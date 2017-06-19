#include "TestB.h"
#include <stdio.h>

namespace ProdB {

TestB::TestB():
  _fsm(*this) {
}

TestB::~TestB() {
}

void TestB::sayEntry(const char * who) {
  Test::say("[wrapped by TestB] ", false);
  Test::sayEntry(who);
}

void TestB::sayExit(const char * who) {
  Test::say("[wrapped by TestB] ", false);
  Test::sayExit(who);
}

}
