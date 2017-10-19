#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define FreeReading 0
#define WriteWaiting 0
#define Writeing 0

// typedef struct queue {
//   
// };
int _mode = FreeReading;
int nReaders = 0;

sem_t _accessQueue;

pthread_mutex_t _mutex;

void readLock(){
  pthread_mutex_lock(&_mutex);

  if(_mode == FreeReading){
    nReaders++;
  }else{
    sem_wait(&_accessQueue);
  }

  pthread_mutex_unlock(&_mutex);
}

void readUnLock(){
  pthread_mutex_lock(&_mutex);

  nReaders--;
  if(nReaders == 0 && _mode == WriteWaiting){
    sem_post(&_accessQueue);
  }

  pthread_mutex_unlock(&_mutex);
}

void writeLock(){
  pthread_mutex_lock(&_mutex);

  if(nReaders == 0){
    sem_wait(&_accessQueue);
  }

  pthread_mutex_unlock(&_mutex);
}

void writeUnLock(){
  pthread_mutex_lock(&_mutex);

  sem_post(&_accessQueue);

  pthread_mutex_unlock(&_mutex);
}

int main(int argc, char *argv[]){

  // sem_init(&_readLock, 0, 1);
  // sem_init(&_writeLock, 0, 1);

}
