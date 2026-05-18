#include "types.h"
#include "stat.h"
#include "user.h"

int lock;
volatile int counter;

#define NLOOP 1000

void
worker(void *arg)
{
  int i, tmp;

  for(i = 0; i < NLOOP; i++){
    mutex_lock(&lock);
    tmp = counter;
    counter = tmp + 1;
    mutex_unlock(&lock);
  }
}

int
main(void)
{
  int t1, t2;

  lock = 0;
  counter = 0;

  t1 = thread_create(worker, 0);
  t2 = thread_create(worker, 0);
  if(t1 < 0 || t2 < 0){
    printf(1, "mutex_test: FAIL create\n");
    exit();
  }

  thread_join(t1);
  thread_join(t2);

  if(counter != 2 * NLOOP){
    printf(1, "mutex_test: FAIL counter=%d\n", counter);
    exit();
  }

  printf(1, "mutex_test: PASS counter=%d\n", counter);
  exit();
}
