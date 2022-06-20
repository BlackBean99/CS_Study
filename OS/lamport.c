#include <stdio.h>
#include <stdlib.h>

#define THREAD_COUNT 10
bool choosing[THREAD_COUNT] = {false};
int number[THREAD_COUNT] = {0};

int number_max(int* numbers, int count) {
  int max = 0;

  for (int i = 0; i < count; i++){
    if (max < numbers[i]) max = numbers[i];
  }

  return max;
}

void lock(int thread) {
  choosing[thread] = true;
  number[thread] = number_max(number, THREAD_COUNT) + 1;
  choosing[thread] = false;
  for (int j = 0; j < THREAD_COUNT; j++) {
    while(choosing[j]);
    while(number[j] && (number[j] < number[thread]));
  }

  return;
}

void unlock(int thread) {
  number[thread] = 0;

  return;
}

int main() {
  
}