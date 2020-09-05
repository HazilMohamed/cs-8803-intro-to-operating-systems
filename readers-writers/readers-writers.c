#include<stdio.h>
#include<pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_READERS 5
#define NUM_READS 5
#define NUM_WRITERS 5
#define NUM_WRITES 5

unsigned int gSharedValue = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gReadPhase = PTHREAD_COND_INITIALIZER;
pthread_cond_t gWritePhase = PTHREAD_COND_INITIALIZER;
int waitReaders = 0, readers = 0;

void *reader(void *threadArg);
void *writer(void *threadArg);

int main() {
  pthread_t read[NUM_READERS], write[NUM_WRITERS];
  int i, readNum[NUM_READERS], writeNum[NUM_WRITERS];

  srandom((unsigned int)time(NULL));

  for(i=0; i<NUM_READERS; i++) {
    readNum[i] = i;
    pthread_create(&read[i], NULL, reader, &readNum[i]);
  }
  for(i=0; i<NUM_WRITERS; i++) {
    writeNum[i] = i;
    pthread_create(&write[i], NULL, writer, &writeNum[i]);
  }
  for(i=0; i<NUM_READERS; i++) {
    pthread_join(read[i], NULL);
  }
  for(i=0; i<NUM_WRITERS; i++) {
    pthread_join(write[i], NULL);
  }
  return 0;
}

void *reader(void *threadArg) {
  int id = *((int*)threadArg);
  int i = 0, numReaders = 0;

  for(i=0; i<NUM_READS;i++){
    usleep(100 * (random()%NUM_READERS+NUM_WRITERS));

    pthread_mutex_lock(&m);
      waitReaders++;
      while(readers == -1) {
        pthread_cond_wait(&gReadPhase, &m);
      }
      waitReaders --;
      numReaders = ++readers;
    pthread_mutex_unlock(&m);

    fprintf(stdout, "[r%d] reading %u [readers: %2d]\n", id, gSharedValue, numReaders);
    pthread_mutex_lock(&m);
      readers--;
      if (readers == 0) {
        pthread_cond_signal(&gWritePhase);
      }
    pthread_mutex_unlock(&m);
  }
  pthread_exit(0);
}


void *writer(void *threadArg) {
  int id = *((int*)threadArg);
  int i =0, numReaders = 0;

  for(i=0; i<NUM_WRITES; i++){
    usleep(100 * (random()%NUM_READERS+NUM_WRITERS));
    pthread_mutex_lock(&m);
      while(readers != 0) {
        pthread_cond_wait(&gWritePhase, &m);
      }      
      readers = -1;
      numReaders = readers;
    pthread_mutex_unlock(&m);

    fprintf(stdout, "[w%d] writing %u* [readers: %2d]\n", id, ++gSharedValue, numReaders);

    pthread_mutex_lock(&m);
      readers = 0;
      if(waitReaders > 0) {
        pthread_cond_broadcast(&gReadPhase);
      } else {
        pthread_cond_signal(&gWritePhase);
      }
    pthread_mutex_unlock(&m);
  }
  pthread_exit(0);
}

