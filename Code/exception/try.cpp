//exception
#include <iostream>
// typeid
#include <typeinfo>

using namespace std;

void exception_int() {
  cout  << '\n' << "Raise exception as int"  << '\n';
  try {
    throw 20;
  } catch (int e) {
    cout << "Exception #"  << e << '\n';
  }
}

void exception_with_str() {
  cout  << '\n' << "Catch exception from std"  << '\n';
  try {
    string toto = "toto";
    string posIsOutOfRange = toto.substr(5 /* out of range */, 10);
  } catch (exception &e) {
    cout << "Exception occur:" << '\n'
         << " - typeid: " << typeid(e).name() << '\n'
         << " - what(): " << e.what() << '\n';
  } catch (...) {
    cout << "Exception occur" << '\n';
  }
}

void no_exception_with_str() {
  cout  << '\n' << "Do not catch exception" << '\n';
  string toto = "toto";
  string posIsOutOfRange = toto.substr(5 /* out of range */, 10);
}

int main(int argc, char *argv[]) {

  exception_int();
  exception_with_str();
  no_exception_with_str();

  return 0;
}
