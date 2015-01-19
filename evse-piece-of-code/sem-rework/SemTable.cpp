#include "SemTable.h"

#define IPC_ALL 0666
#define SEM_TABLE_SIZE 250


SemTable():
  _isPrivateInitialized(false),
  _lastSemNumberUsed(0),
  _semTable(-1)
{
  Init();
}


~SemTable(){
 
  LOG_ENTER_;

  // TODO : check class Sem do the same only for public one !!!
  
  if (_semTable > 0)
    {
      res = semctl(/* Id */ _sem, 
                   /* Num : NA here as the action is done on the entir set of semaphore */ 0, 
                   /* Cmd */ IPC_RMID);
    }
}

void SemTable::Init() {

  key_t key;
  
  // Init sem table for private semaphore
  key = IPC_PRIVATE;

  /**
   *  (IPC_CREAT) | (IPC_EXCL) flags are usefull to protect from already exist file
   */
  _semTable = semget(/* Key */ key,
                /* Num */ SEM_TABLE_SIZE,
                /* Flags */ (IPC_CREAT) | (IPC_EXCL) | (IPC_ALL) |
                S_IRWXU | S_IRWXG | S_IRWXO
                );     
     
  _isPrivateInitialized = true;
}

void  SemTable::getSem(int &semTable, int &semNumber){
 
  semTable = _semTable;

  Lock();
  semNumber = (++_lastSemNumberUsed);
  Unlock();

}

void SemTable::Lock() {

  if(_semTable > 0) {
    _semOp.Lock(_semTable, 0)
  } else {
    printf("FATAL ERROR: sem table not initialized\n");
    exit(0);
  }

}

void SemTable::Unlock() {

  if(_semTable > 0) {
    _semOp.Unlock(_semTable, 0)
  } else {
    printf("FATAL ERROR: sem table not initialized\n");
    exit(0);
  }

}
