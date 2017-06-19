#include "Toto.h"
#include "Totos.h"
#include "TotoPrivate.h"

Toto::Toto(int param1, int param2):
  _p(*new TotoPrivate(
       *new Toto1(param1),
       *new Toto2(param2)))
{}

int Toto::getVal1() {
  return _p._toto1.getVal();
}

int Toto::getVal2() {
  return _p._toto2.getVal();
}
