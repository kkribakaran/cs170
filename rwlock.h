//#include<stdio.h>
//#include<pthread.h>
#ifndef RWLOCK_H
#define RWLOCK_H

#include <semaphore.h>
#include <mutex>
#include <condition_variable>
#include <pthread.h>

class RWLock {
  private:
    pthread_mutex_t rwMutex;
    pthread_cond_t readGo;
    pthread_cond_t writeGo;

    int activeReaders;
    int activeWriters;
    int waitingReaders;
    int waitingWriters;

    bool readShouldWait();
    bool writeShouldWait();


  public:
    RWLock();
    ~RWLock();
    //Reader
    void startRead();
    void doneRead();
    //Writer    
    void startWrite();
    void  doneWrite();
};
#endif