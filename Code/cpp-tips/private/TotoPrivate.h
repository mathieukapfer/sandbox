#ifndef TOTOPRIVATE_H
#define TOTOPRIVATE_H

class Toto1;
class Toto2;

class TotoPrivate {
 public:
  TotoPrivate(Toto1 &toto1, Toto2 &toto2):
    _toto1(toto1),
    _toto2(toto2) {}

  virtual ~TotoPrivate() {};

  Toto1 &_toto1;
  Toto2 &_toto2;

};

#endif /* TOTOPRIVATE_H */
