#include "ClassWithEnum.h"

ClassWithEnum::ClassWithEnum() {
  cout << "ClassWithEnum:" << sizeof(ClassWithEnum) << endl;
  cout << "&start:" << &start << endl;
  cout << "&enum1:" << &enum1 << endl;
  cout << "&enum2:" << &enum2 << endl;
  cout << "&end  :" << &end << endl;
}


ClassWithEnum::~ClassWithEnum() {};
