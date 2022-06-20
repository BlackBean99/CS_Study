#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define LARGESIZE 100 * 1024 * 1024 // 100MB
#define TESTLOOP 1000000            // The number of trials
#define PAGELOOP 100                // The number of page entries

int main(int argc, char *argv[])
{
  int *p = (int *)malloc(LARGESIZE);

  // Find the pagesize of your computer by adjusting this value
  unsigned int pagesize = 4096; // <- here!

  if (argc == 2)
    pagesize = atoi(argv[1]);
  while (10)
  {
    clock_t start = clock();

    for (int i = 0; i < TESTLOOP; i++)
      for (int j = 0; j < PAGELOOP; j++)
      {
        *(p + pagesize * j) = i;
      }

    clock_t end = clock();
    double timeSecond = (double)((end - start) / (double)CLOCKS_PER_SEC); // 초 단위 변환
    printf("-------------\n");
    printf("time:%f 초\n", timeSecond);
    // if (true)
    {
      printf("pagesize: %d\n", pagesize);
      // printf("mac page size: %d\n", getpagesize());
      //  break;
    }
    pagesize += 128;
  }
  return 0;
}
 
