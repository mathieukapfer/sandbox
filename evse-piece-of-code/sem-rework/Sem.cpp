#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <string.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Sem.h"
#include "SemCollection.h"

#include "TraceDebug.h"
#include "stdio.h"

#include <syscall.h>

int gettid()
{
    int tid = syscall(SYS_gettid);
}

TRACEDEBUG_REGISTER("[KERNEL][SEM]");

#define IPC_ALL 0666



bool Sem::_initialized = false;
const char * Sem::_baseName = "/tmp/Sem";

Sem::Sem(const char *name, bool isPrivate, SemBehavour behavour) :
_sem(0),
_behavour(behavour),
_mutexCount(0),
_pid_owner(0),
_tid_owner(0),
_pid_waiter(0),
_tid_waiter(0)

{
    _name = name;
    Create(isPrivate);

    SemCollection::inst()->registerSem(this);

}

Sem::~Sem()
{
    LOG_INFO(("Destructor called for %s\n", _name));

    if(_isPrivate) {
      Remove();
    }
}

void Sem::Create(bool isPrivate)
{
    struct semid_ds semid_ds;
    key_t key;
    
    // WARNING : DO NOT ENABLE ANY LOG HEAR
    // It will trigger recursive call for the first sem creation for logserver
    // 
    // !!! LOG_INFO(("ENTER Create sem %s\n", _name)); !!!

    _isPrivate = isPrivate;

    if (isPrivate)
    {
        key = IPC_PRIVATE;
    }
    else
    {
        if (!_initialized)
        {
            CreateDirectory();
            _initialized = true;
        }

        key = File_To_Key();
    }


    if (isPrivate)
    {
        /**
         *  (IPC_CREAT) | (IPC_EXCL) flags are usefull to protect from already exist file
         */
        _sem = semget(/* Key */ key,
                      /* Num */ 1,
                      /* Flags */ (IPC_CREAT) | (IPC_EXCL) | (IPC_ALL) |
                      S_IRWXU | S_IRWXG | S_IRWXO
                      );
    }
    else
    {

        /**
         * do not need (IPC_CREAT) | (IPC_EXCL) flags protection,
         * the aim is to use an already defined sem.
         */

        _sem = semget(/* Key */ key,
                      /* Num */ 1,
                      /* Flags */ (IPC_CREAT) | (IPC_ALL) |
                      S_IRWXU | S_IRWXG | S_IRWXO
                      );
    }
    if (_sem < 0)
    {
        LOG_FATAL(("Create failed for '%s', %s (errno = %d)\n", _name, strerror(errno), errno));
        exit(0);
    }

    // initialize permission 
#if 0
    if (semctl(_sem, /* semnum */ 0, IPC_STAT, &semid_ds) < 0)
        LOG_ERROR(("semctl call failed (errno %d)\n", errno));

    semid_ds.sem_perm.mode = 0777;
    if (semctl(_sem, /* semnum */ 0, IPC_SET, semid_ds) < 0)
        LOG_ERROR(("semctl call failed (errno %d)\n", errno));
#endif

    // initialize sem value to 1
    if (semctl(_sem, /* semnum */ 0, SETVAL, 1) < 0)
        LOG_ERROR(("semctl call failed (errno %d)\n", errno));

    // LOG_DEBUG(("Create Sem %s - id %d\n", _name, _sem));

    // BASH : 
    // cmd to get sem info
    //  for id in `ipcs -s | cut -d ' ' -f 2 ` ; do ipcs -si $id; done   
    // cmd to rm all sem
    //  for id in `ipcs |  cut -d ' ' -f 2 ` ; do ipcrm -s$id; done
}

const void Sem::ResetValue()
{
    LOG_ENTER;

    if (semctl(_sem, /* semnum */ 0, SETVAL, 0) < 0)
        LOG_ERROR(("semctl call failed (errno %d)\n", errno));

}

const int Sem::Lock()
{
    int res;

    struct sembuf op;

    int tid_waiter = 0;
    int pid_waiter = 0;

    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = 0;

    // use local variable to be thread safe
    tid_waiter = (int) gettid();
    pid_waiter = (int) getpid();

    LOG_DEBUG(("ENTER Lock sem %s tried by tid:%d id:%d \n", _name, tid_waiter, _sem));

    if ((tid_waiter == _tid_owner) && (pid_waiter == _pid_owner) && (_behavour == SEM_BEHAVOUR_MUTEX))
    {
        LOG_DEBUG(("Process pid=%d tid=%d id:%d try to lock again the same MUTEX '%s' (useless)\n",
                   _pid_owner, _tid_owner, _sem, _name));

        _mutexCount++;

        res = 0;
    }
    else
    {

        // save in instance attribute for debug purpose
        _tid_waiter = tid_waiter;
        _pid_waiter = pid_waiter;


        while (1)
        {
            res = semop(/* Id    */ _sem,
                        /* Sops  */ &op,
                        /* Nsops */ 1);

            if (res == -1)
            {
                if (errno != EINTR)
                {
                    LOG_FATAL(("Lock  failed for '%s', %s (errno = %d)\n", _name, strerror(errno), errno));

                    if (errno == 13)
                    {
                        LOG_INFO(("Try in superuser mode !!! \n"));
                        exit(0);
                    }

                    break;
                }
            }
            else
            {
                break;
            }
        }

        _tid_owner = (int) gettid();
        _pid_owner = (int) getpid();

        LOG_DEBUG(("      Lock sem %s done by tid:%d id:%d\n", _name, (int) _tid_owner, _sem));

    }

    return res;

}

const void Sem::Unlock()
{
    int res;
    int tid, pid;

    struct sembuf op;

    LOG_DEBUG(("ENTER Unlock sem %s\n", _name));

    // check owner
    // use local variable to be thread safe
    tid = (int) gettid();
    pid = (int) getpid();

    if (_behavour == SEM_BEHAVOUR_MUTEX)
        if ((tid == _tid_owner) && (pid == _pid_owner))
        {
            if (_mutexCount > 0)
            {
                // do not unlock the MUTEX, but just decrease the counter 
                LOG_DEBUG(("Decrease mutex count\n"));
                _mutexCount--;
                return;
            }
        }
        else if (_isPrivate) {
          // If semaphore is private (same process), do additional check:
          // you are not the owner, some thing is not clear, log it !
          LOG_WARNING(("Process pid=%d tid=%d id =%d try to unlock semaphore'%s' it doesn't have. \n",
                       pid, tid, _sem, _name));
          
          LOG_INFO(("Owner is pid=%d tid=%d id = %d \n",
                    _pid_owner, _tid_owner, _sem));
                  

        }


    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = 0;

    _tid_owner = 0;
    _pid_owner = 0;

    res = semop(/* Id    */ _sem,
                /* Sops  */ &op,
                /* Nsops */ 1);

}

void Sem::CreateDirectory()
{

    LOG_ENTER_;

    // check if directory already exist
    struct stat st;
    bool needCreation = false;
    if (stat(_baseName, &st) == 0)
    {
        if (st.st_mode & S_IFDIR != 0)
        {
            LOG_INFO(("directory %s already exist \n", _baseName));
        }
        else // else create it
        {
            needCreation = true;
        }
    }
    else
    {
        LOG_ERROR(("Can not stat directory %s\n", _baseName));
        needCreation = true;
    }
    if (needCreation)
    {
        // create directory with permission : is a directory + user R/W/X + group R/W/X + other R/W/X
        if (mkdir(_baseName, S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO) == 0)
        {
            LOG_INFO(("Creation of directory %s with all rigths :%d\n", _baseName, S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO));
        }
        else
        {
            LOG_ERROR(("Error while creating directory %s\n", _baseName));
        }
    }
    LOG_EXIT_((""));

}

key_t Sem::File_To_Key()
{
    char File [256];
    int Fd;
    int Result;

    strcpy(File, _baseName);
    strcat(File, "/");
    strcat(File, _name);
    Fd = open(File, (O_RDWR | O_CREAT), 777);
    if (Fd > 0)
        Result = close(Fd);
    return ftok(File, 1);
}

void Sem::Remove()
{
    char File [256];
    int res;

    if (_sem > 0)
    {

        res = semctl(/* Id */ _sem, /* Num */ 0, /* Cmd */ IPC_RMID);

        /* Delete the associated file */
        strcpy(File, _baseName);
        strcat(File, _name);
        res = unlink(File);

        _sem = 0;
    }
}

void Sem::Monitor()
{

    log(" %-15s : (%6d,%6d) - (%6d,%6d) - %s(%d)\n", _name,
        _pid_owner, _tid_owner, _pid_waiter, _tid_waiter,
        (_behavour == 1 ? "TOKEN" : "MUTEX"), _mutexCount);

}

const char* Sem::GetName()
{
    return _name;
}
