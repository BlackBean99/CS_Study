 #include <stdio.h>
 #include <stdlib.h>
 #include <stdbool.h>
 #include <pthread.h>
  const int count = 200000;
 
  int sum = 0; // global variable, shared data
  pthread_spinlock_t lock;
 
  void* myThread1(void *p) {
  for(int i = 0; i < count; i++) {
  pthread_spin_lock(&lock); // entry
  sum += 1;
  pthread_spin_unlock(&lock); // exit
  }
  return 0;
  }
 
  void* myThread2(void *p) {
  for(int i = 0; i < count; i++) {
  pthread_spin_lock(&lock); // entry
  sum -= 1;
  pthread_spin_unlock(&lock); // exit
  }
  return 0;
  }
 
  int main() {
  pthread_t tid1, tid2; // thread id
  int count = 200000;
  int *ret1, *ret2;
 
  printf("Start!\n");
 
  pthread_spin_init(&lock, NULL); // init spin
 
  pthread_create(&tid1, NULL, myThread1, NULL);
  pthread_create(&tid2, NULL, myThread2, NULL);
  pthread_join(tid1, (void**)&ret1); // waiting for 'tid1'
  pthread_join(tid2, (void**)&ret2); // waiting for 'tid2'
 
  printf("sum = %d\n", sum);
 
 pthread_spin_destroy(&lock); // destroy spin
 return 0;
 }