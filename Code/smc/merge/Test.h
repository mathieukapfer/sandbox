#ifndef TEST_H
#define TEST_H

class Test {
 public:
  Test();
  virtual ~Test();

  // commun services
  virtual void sayEntry(const char * who);
  virtual void sayExit(const char * who);
  virtual void say(const char * something, bool newline = true);

};
#endif /* TEST_H */
