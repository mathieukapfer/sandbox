#ifndef TOTOS_H
#define TOTOS_H

class Toto1 {
 public:
  explicit Toto1(int val):_val(val) {};
  virtual ~Toto1() {};
  int getVal() {return _val;};
 private:
  int _val;
};

class Toto2 {
 public:
  explicit Toto2(int val):_val(val) {};
  virtual ~Toto2() {};
  int getVal() {return _val;};
 private:
  int _val;
};


#endif /* TOTOS_H */
