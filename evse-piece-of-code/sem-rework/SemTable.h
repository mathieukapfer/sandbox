#ifndef SEMTABLE_H
#define SEMTABLE_H

class SemTable
{
public:
  SemTable();
  virtual ~SemTable();

  void getSem(int &semTable, int &semNumber);

private:

  Lock();
  Unlock();

  // for private sem
  bool _isPrivateInitialized;
  int  _lastSemNumberUsed;
  int  _semTable;

};


#endif /* SEMTABLE_H */
