#include <iostream>

using namespace std;

class ClassWithEnum {
 public:
  ClassWithEnum();
  virtual ~ClassWithEnum();

  typedef enum {
    VAL1,
    VAL2
  } MY_ENUM;

  int start;
  MY_ENUM enum1;
  MY_ENUM enum2;
  int end;
};
