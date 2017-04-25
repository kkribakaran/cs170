#include <iostream>
#include "phash.h"

LinkedHashEntry:: LinkedHashEntry(int key, int value) {
            this->key = key;
            this->value = value;
            this->next = NULL;
      }

int LinkedHashEntry:: getKey() {
            return key;
      }
      
int LinkedHashEntry:: getValue() {
            return value;
      }
 
void LinkedHashEntry:: setValue(int value) {
            this->value = value;
      }
 

LinkedHashEntry * LinkedHashEntry:: getNext() {
            return next;
      }
 
void LinkedHashEntry:: setNext(LinkedHashEntry *next) {
            this->next = next;
      }


const int TABLE_SIZE = 128;
 
HashMap::HashMap() {
            table = new LinkedHashEntry*[TABLE_SIZE];
            #if RWLOCK
              lock = new RWLock*[TABLE_SIZE];
            #else
              lock = new pthread_mutex_t*[TABLE_SIZE];
            #endif 
            for (int i = 0; i < TABLE_SIZE; i++) {
                  table[i] = NULL;
                  #if RWLOCK
                    RWLock* curr_lock = new RWLock();
                  #else
                    pthread_mutex_t* curr_lock = new pthread_mutex_t();
                    pthread_mutex_init(curr_lock, NULL);
                  #endif
                  lock[i] = curr_lock;
            }
      }

int HashMap:: get(int key) {
            
            int hash = (key % TABLE_SIZE);
            int result;
            #ifdef RWLOCK
              lock[hash]->startRead();
            #else 
              pthread_mutex_lock(lock[hash]);
            #endif
            if (table[hash] == NULL) {
                  result = -1;
            } else {
                  LinkedHashEntry *entry = table[hash];
                  while (entry != NULL && entry->getKey() != key)
                        entry = entry->getNext();
                  if (entry == NULL)
                        result = -1;
                  else
                        result = entry->getValue();
            }

            #ifdef RWLOCK
              lock[hash]->doneRead();
            #else 
              pthread_mutex_unlock(lock[hash]);
            #endif
            return result;
      }
 
void HashMap::put(int key, int value) {

            int hash = (key % TABLE_SIZE);
            #ifdef RWLOCK
              lock[hash]->startWrite();
            #else 
              pthread_mutex_lock(lock[hash]);
            #endif
            if (table[hash] == NULL)
                  table[hash] = new LinkedHashEntry(key, value);
            else {
                  LinkedHashEntry *entry = table[hash];
                  while (entry->getNext() != NULL)
                        entry = entry->getNext();
                  if (entry->getKey() == key)
                        entry->setValue(value);
                  else
                        entry->setNext(new LinkedHashEntry(key, value));
            }

            #ifdef RWLOCK
              lock[hash]->doneWrite();
            #else 
              pthread_mutex_unlock(lock[hash]);
            #endif
      }
 

void HashMap:: remove(int key) {

            int hash = (key % TABLE_SIZE);
            #ifdef RWLOCK
              lock[hash]->startWrite();
            #else 
              pthread_mutex_lock(lock[hash]);
            #endif
            if (table[hash] != NULL) {
                  LinkedHashEntry *prevEntry = NULL;
                  LinkedHashEntry *entry = table[hash];
                  while (entry->getNext() != NULL && entry->getKey() != key) {
                        prevEntry = entry;
                        entry = entry->getNext();
                  }
                  if (entry->getKey() == key) {
                        if (prevEntry == NULL) {
                             LinkedHashEntry *nextEntry = entry->getNext();
                             delete entry;
                             table[hash] = nextEntry;
                        } else {
                             LinkedHashEntry *next = entry->getNext();
                              delete entry;
                             prevEntry->setNext(next);
                        }
                  }
            }

            #ifdef RWLOCK
              lock[hash]->doneWrite();
            #else 
              pthread_mutex_unlock(lock[hash]);
            #endif
      }
 
HashMap:: ~HashMap() {
            for (int i = 0; i < TABLE_SIZE; i++)
                  if (table[i] != NULL) {
                        LinkedHashEntry *prevEntry = NULL;
                        LinkedHashEntry *entry = table[i];
                        while (entry != NULL) {
                             prevEntry = entry;
                             entry = entry->getNext();
                             delete prevEntry;
                        }
                  }
                  delete lock[i];

            delete[] table;
            delete[] lock;
      }
