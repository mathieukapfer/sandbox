#include "TestDefault.h"

int main(int argc, char *argv[])
{
  TestDefault test;
  test._fsm.setDebugFlag(true);

  test._fsm.waiting();
  test._fsm.goMaintenance();
  test._fsm.goBack();

  return 0;
}
