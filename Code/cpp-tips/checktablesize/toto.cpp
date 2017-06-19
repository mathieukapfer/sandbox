
#include "CAssert.h"

enum {
  VAL1,
  VAL2,
  VAL3,
  VAL_MAX
};

int table[] = { 1, 2, 3, 4 };

CASSERT(sizeof(table) / sizeof(int) == VAL_MAX, toto_cpp);
STATIC_ASSERT(1)

int main(int argc, char *argv[])
{   
  return 0;
}
