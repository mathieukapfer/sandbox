#include <vector>
#include <iostream>

#define TEST_THIS(cmd) std::cout << #cmd << ":" << (cmd) << std::endl;

using namespace std;

int main(int argc, char *argv[]) {
  std::vector<int> myVector(100);

  TEST_THIS(myVector.size());
  TEST_THIS(myVector.empty());
  TEST_THIS(*myVector.begin());
  TEST_THIS(myVector.end() - myVector.begin());

  myVector.push_back(99);
  TEST_THIS(myVector.back()); // no '*'
  TEST_THIS(myVector.size());

  return 0;
}
