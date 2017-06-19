#ifndef TOTO_H
#define TOTO_H

class TotoPrivate;

class Toto {
 public:
  Toto(int param1, int param2);
  virtual ~Toto() {};
  int getVal1();
  int getVal2();

 private:
  TotoPrivate & _p;
};



#endif /* TOTO_H */
