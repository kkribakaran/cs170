/**
 * Code is based on
 *http://www.algolist.net/Data_structures/Hash_table/Chaining
 *
 **/

#include <mutex>
#include <pthread.h>
#include "rwlock.h"

class LinkedHashEntry {
  private:
    int key;
    int value;
    LinkedHashEntry *next;
    
  public:
    LinkedHashEntry(int key, int value); 
    int getKey(); 
    int getValue();
    void setValue(int value);

    LinkedHashEntry *getNext(); 
    void setNext(LinkedHashEntry *next); 
};


class HashMap {
  private:
    #if defined(FINEGRAIN) && defined(RWLOCK)
      RWLock **lock;
    #elif defined(FINEGRAIN) && !defined(RWLOCK)
      pthread_mutex_t **lock;
    #elif !defined(FINEGRAIN) && defined(RWLOCK)
      RWLock lock;
    #else 
      pthread_mutex_t lock;
    #endif
    LinkedHashEntry **table;
  public:
    HashMap(); 
    int get(int key); 
    void put(int key, int value); 
    void remove(int key); 
    ~HashMap(); 
};

