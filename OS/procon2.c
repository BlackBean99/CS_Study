#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define N_COUNTER 4 // the size of a shared buffer
#define MILLI 1000  // time scale

#define THREAD_COUNT 2
#define barrier() asm("mfence")

//
bool choosing[THREAD_COUNT] = {false};

int number[THREAD_COUNT] = {0};

int get_max(int *numbers, int count)
{
  int max = 0;

  for (int i = 0; i < count; i++)
  {
    if (max < numbers[i])
      max = numbers[i];
  }

  return max;
}

void lock(int thread)
{
  barrier();
  choosing[thread] = true;
  number[thread] = get_max(number, THREAD_COUNT) + 1;
  choosing[thread] = false;
  for (int j = 0; j < THREAD_COUNT; j++)
  {
    while (choosing[j])
      ;
    while (number[j] && (number[j] < number[thread]))
      ;
  }
  barrier();

  return;
}

void unlock(int thread)
{
  barrier();

  number[thread] = 0;
  barrier();

  return;
}

void mywrite(int n);
int myread();

// pthread_mutex_t critical_section; // POSIX mutex
sem_t semWrite, semRead; // POSIX semaphore
int queue[N_COUNTER];    // shared buffer
int wptr;                // write pointer for queue[]
int rptr;                // read pointer for queue[]

// producer thread function
void *producer(void *arg)
{
  for (int i = 0; i < 1000; i++)
  {
    mywrite(i); /**** write i into the shared memory ****/
    printf("producer : wrote %d\n", i);

    // sleep m miliseconds
    int m = rand() % 10;
    usleep(MILLI * m * 10); // m*10
  }
  return NULL;
}

// consumer thread function
void *consumer(void *arg)
{
  for (int i = 0; i < 1000; i++)
  {
    int n = myread(); /**** read a value from the shared memory ****/
    printf("\tconsumer : read %d\n", i);

    // sleep m miliseconds
    int m = rand() % 10;
    usleep(MILLI * m * 10); // m*10
  }
  return NULL;
}

// write n into the shared memory
void mywrite(int n)
{
  /* [Write here] */
  sem_wait(&semWrite);

  // pthread_mutex_lock(&critical_section);
  lock(1);
  queue[wptr++] = n;
  if (wptr == N_COUNTER)
    wptr = 0;
  // pthread_mutex_unlock(&critical_section);
  unlock(1);

  sem_post(&semRead);
}

// write a value from the shared memory
int myread(){
  /* [Write here] */
  int n;
  sem_wait(&semRead);
  
  // pthread_mutex_lock(&critical_section);
  lock(0);
  n = queue[rptr++];
  if (rptr == N_COUNTER)
    rptr = 0;
  // pthread_mutex_unlock(&critical_section);
  unlock(0);

  sem_post(&semWrite);

  return n;
}

int main()
{
  pthread_t t[2]; // thread structure
  srand(time(NULL));

  // pthread_mutex_init(&critical_section, NULL); // init mutex

  // init queue
  wptr = 0;
  rptr = 0;

  // init semaphore
  /* [Write here] */
  sem_init(&semWrite, 0, 4);
  sem_init(&semRead, 0, 0);

  // create the threads for the producer and consumer
  pthread_create(&t[0], NULL, producer, NULL);
  pthread_create(&t[1], NULL, consumer, NULL);

  clock_t start = clock();
  for (int i = 0; i < 2; i++)
    pthread_join(t[i], NULL); // wait for the threads
  clock_t end = clock();
  double timeSecond = (double)((end - start) / (double)CLOCKS_PER_SEC); // 초 단위 변환
  printf("time:%f 초\n", timeSecond);

  // destroy the semaphores
  /* [Write here] */
  sem_destroy(&semWrite);
  sem_destroy(&semRead);

  // pthread_mutex_destroy(&critical_section); // destroy mutex
  return 0;
}
