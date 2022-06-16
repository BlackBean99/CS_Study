#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem;
 void* myThread1(void *p) {
 int cnt = -1;

 sem_wait(&sem); // P
 sem_getvalue(&sem, &cnt); // get the semaphore cnt
 printf("%s uses the resource. Remaining %d \n", (char*)p, cnt);
 sleep(1);
 sem_post(&sem); // V

 sem_getvalue(&sem, &cnt); // get the semaphore cnt
 printf("%s finished using the resource. Remaining %d \n", (char*)p, cnt);
 }

 int main() {
 int cnt = -1;
 pthread_t tid[5];
 char *name[] = {"t1", "t2", "t3", "t4", "t5" };

 printf("Start!\n");

 sem_init(&sem, 0, 3); // set the semaphore count to 3
 sem_getvalue(&sem, &cnt);
 printf("1. Semaphore: %d \n", cnt);

 for(int i = 0; i < 5 ; i++)
 pthread_create(&tid[i], NULL, myThread1, (void*)name[i]);
 for(int i = 0; i < 5 ; i++)
 pthread_join(tid[i], NULL);

 sem_getvalue(&sem, &cnt);
 printf("2. Semaphore: %d \n", cnt);

 sem_destroy(&sem);
 return 0;
 }