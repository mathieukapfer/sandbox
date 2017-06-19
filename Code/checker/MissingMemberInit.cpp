#include "MissingMemberInit.h"

/*
  >cppcheck --enable=warning MissingMemberInit.cpp
   Checking MissingMemberInit.cpp...
   [MissingMemberInit.cpp:3]: (warning) Member variable 'MissingMemberInit::_a_param' is not initialized in the constructor.
*/

MissingMemberInit::MissingMemberInit(int a) {
};
