#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int first, second;

  first = kthread_test();
  second = kthread_test();

  if(first <= 0 || second <= first){
    printf(1, "kthread_test: FAIL first=%d second=%d\n", first, second);
    exit();
  }

  printf(1, "kthread_test: PASS first=%d second=%d\n", first, second);
  exit();
}
