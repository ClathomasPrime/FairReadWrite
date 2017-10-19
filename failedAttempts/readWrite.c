#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

sem_t _readLock;
int nReaders;
sem_t _writeLock;
int nWriters;
pthread_mutex_t _mutex;

void readLock(){
  pthread_mutex_lock(&_mutex);

  if(nReaders == 0){ //first reader
    sem_wait(&_writeLock);
  }

  nReaders++;

  if(nWriters > 0){
    sem_wait(&_readLock);
  }

  pthread_mutex_unlock(&_mutex);
}

void readUnLock(){
  pthread_mutex_lock(&_mutex);

  nReaders--;

  if(nReaders == 0){ //last reader
    sem_post(&_writeLock);
  }

  pthread_mutex_unlock(&_mutex);
}

void writeLock(){
  pthread_mutex_lock(&_mutex);

  nWriters++;
  if(nReaders > 0){
    sem_wait(&_writeLock);
  }

  pthread_mutex_unlock(&_mutex);
}

void writeUnLock(){
  pthread_mutex_lock(&_mutex);

  nWriters--;

  pthread_mutex_unlock(&_mutex);
}

int main(int argc, char *argv[]){

  sem_init(&_readLock, 0, 1);
  sem_init(&_writeLock, 0, 1);

}
