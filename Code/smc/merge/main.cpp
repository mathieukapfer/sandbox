#include "TestA.h"
#include "TestB.h"

int main(int argc, char *argv[])
{
  ProdA::TestA testA;
  ProdB::TestB testB;

  testA._fsm.leaveInit();
  testA._fsm.backToInit();

  testA._fsm.goInside();
  testA._fsm.goState2();
  testA._fsm.goBack();

  testB._fsm.leaveInit();
  testB._fsm.backToInit();

  testB._fsm.goInside();
  testB._fsm.goState2();
  testB._fsm.goBack();

  return 0;
}
