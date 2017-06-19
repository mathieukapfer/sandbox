#include <iostream>
#include <string.h>

using namespace std;

char * module[] = {
  "[MAIN][SUBMODUL]",
  "[MAIN][SUBMODUL]",
  "[MAIN]" };

char * getMainStr(char * label) {
  *strchrnul(label, ']') = '\0';
  return label;
}

int main(int argc, char *argv[]) {
  int index = 0;
  do {
    char buf[100];
    strncpy(buf, module[index], 100);
    std::cout << getMainStr(buf) << std::endl;
  }   while (++index < sizeof(module)/sizeof(char *));
  return 0;
}
