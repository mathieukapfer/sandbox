#include "Test.h"
#include <stdio.h>

Test::Test(){
}

Test::~Test() {
}

void Test::sayEntry(const char * who) {
  printf("Entry %s\n", who);
}

void Test::sayExit(const char * who) {
  printf("Exit %s\n", who);
}

void Test::say(const char * something, bool newline) {
  printf("%s", something);
  if (newline) {
    printf("\n");
  }
}
