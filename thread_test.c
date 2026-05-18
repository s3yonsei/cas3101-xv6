#include "types.h"
#include "stat.h"
#include "user.h"

int result;

void
worker(void *arg)
{
  result = (int)arg;
}

int
main(void)
{
  int tid;

  result = 0;
  tid = thread_create(worker, (void*)42);
  if(tid < 0){
    printf(1, "thread_test: FAIL create\n");
    exit();
  }

  if(thread_join(tid) < 0){
    printf(1, "thread_test: FAIL thread_join\n");
    exit();
  }

  if(result != 42){
    printf(1, "thread_test: FAIL result=%d\n", result);
    exit();
  }

  printf(1, "thread_test: PASS result=%d\n", result);
  exit();
}
