#include "Toto.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
  Toto toto(888,999);

  cout << toto.getVal1() << endl;
  cout << toto.getVal2() << endl;
  
  return 0;
}
