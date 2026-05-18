#include "types.h"
#include "stat.h"
#include "user.h"

int lock;
int cv;
int ready;
int started;
int done;
int wait_result;

void
waiter(void *arg)
{
  (void)arg;
  mutex_lock(&lock);
  started = 1;
  while(ready == 0)
    wait_result = cond_wait(&cv, &lock);
  done = 1;
  mutex_unlock(&lock);
}

int
main(void)
{
  int tid;

  lock = 0;
  cv = 0;
  ready = 0;
  started = 0;
  done = 0;
  wait_result = -1;

  tid = thread_create(waiter, 0);
  if(tid < 0){
    printf(1, "cond_test: FAIL create\n");
    exit();
  }

  while(started == 0)
    sleep(1);
  sleep(5);

  if(done != 0){
    printf(1, "cond_test: FAIL did not block\n");
    exit();
  }

  mutex_lock(&lock);
  ready = 1;
  cond_signal(&cv);
  mutex_unlock(&lock);
  thread_join(tid);

  if(done != 1 || wait_result != 0){
    printf(1, "cond_test: FAIL wake\n");
    exit();
  }

  printf(1, "cond_test: PASS\n");
  exit();
}
