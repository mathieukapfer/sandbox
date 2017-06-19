#include "TestDefault.h"
#include <stdio.h>


TestDefault::TestDefault():
  _fsm(*this) {
}

TestDefault::~TestDefault() {
}

void TestDefault::say(const char * something, bool newline) {
  printf("%s", something);
  if (newline) {
    printf("\n");
  }
}

const char * TestDefault::getState() {
  return _fsm.getState().getName();
}

bool TestDefault::displayState() {
  printf("current state: %s\n", getState());
  return true;
}
