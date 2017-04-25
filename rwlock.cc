#include <pthread.h>

#include "rwlock.h"

using namespace std;

RWLock::RWLock() {
	activeReaders = 0;
  activeWriters = 0;
  waitingReaders = 0;
  waitingWriters = 0;
  pthread_mutex_init(&rwMutex, NULL);
  pthread_cond_init(&readGo, NULL);
  pthread_cond_init(&writeGo, NULL);
}

RWLock::~RWLock() {
	pthread_mutex_destroy(&rwMutex);
	pthread_cond_destroy(&readGo);
	pthread_cond_destroy(&writeGo);
}

//Reader
void RWLock::startRead() {
	pthread_mutex_lock(&rwMutex);
	waitingReaders++;
	while (readShouldWait()) {
		pthread_cond_wait(&readGo, &rwMutex);
	}
	waitingReaders--;
	activeReaders++;
	pthread_mutex_unlock(&rwMutex);
}

void RWLock::doneRead() {
	pthread_mutex_lock(&rwMutex);
	activeReaders--;
	if (activeReaders == 0 && waitingWriters > 0) {
		pthread_cond_signal(&readGo);
	}
	pthread_mutex_unlock(&rwMutex);
}
//Writer    
void RWLock::startWrite() {
	pthread_mutex_lock(&rwMutex);
	waitingWriters++;
	while (writeShouldWait()) {
		pthread_cond_wait(&writeGo, &rwMutex);
	}
	waitingWriters--;
	activeWriters++;
	pthread_mutex_unlock(&rwMutex);
}

void RWLock::doneWrite() {
	pthread_mutex_lock(&rwMutex);
	activeWriters--;
	if (activeWriters == 0 && waitingWriters > 0) {
		pthread_cond_signal(&writeGo);
	}
	pthread_mutex_unlock(&rwMutex);
}

bool RWLock::readShouldWait() {
	return (activeWriters > 0 || waitingWriters > 0);
}

bool RWLock::writeShouldWait() {
	return (activeReaders > 0 || activeWriters > 0);
}