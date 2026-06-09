#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "iouring.h"

volatile int progress;

int
main(void)
{
  int fd, n, i;
  int expected[4];
  int seen[4];
  char a[] = "async-";
  char b[] = "worker-";
  char c[] = "done";
  char expect[] = "async-worker-done";
  char out[32];
  struct io_uring ring;
  struct io_uring_cqe cqe;

  unlink("ior_async");
  fd = open("ior_async", O_CREATE | O_RDWR);
  if(fd < 0 || iouring_setup(&ring) < 0){
    printf(1, "iouring_async_test: FAIL setup\n");
    exit();
  }

  if(io_uring_prep(&ring, IORING_OP_WRITE, fd, a, strlen(a), 1) < 0 ||
     io_uring_prep(&ring, IORING_OP_WRITE, fd, b, strlen(b), 2) < 0 ||
     io_uring_prep(&ring, IORING_OP_WRITE, fd, c, strlen(c), 3) < 0){
    printf(1, "iouring_async_test: FAIL prep\n");
    exit();
  }

  for(i = 0; i < 20000; i++)
    progress += i & 3;
  if(progress == 0){
    printf(1, "iouring_async_test: FAIL progress\n");
    exit();
  }

  expected[1] = strlen(a);
  expected[2] = strlen(b);
  expected[3] = strlen(c);
  memset(seen, 0, sizeof(seen));

  for(i = 0; i < 3; i++){
    if(io_uring_wait_cqe(&ring, &cqe) < 0){
      printf(1, "iouring_async_test: FAIL wait\n");
      exit();
    }
    if(cqe.tag < 1 || cqe.tag > 3 ||
       seen[cqe.tag] || cqe.res != expected[cqe.tag]){
      printf(1, "iouring_async_test: FAIL cqe res=%d tag=%d\n",
             cqe.res, cqe.tag);
      exit();
    }
    seen[cqe.tag] = 1;
  }

  iouring_close(&ring);
  close(fd);

  fd = open("ior_async", O_RDONLY);
  memset(out, 0, sizeof(out));
  n = read(fd, out, sizeof(out));
  close(fd);
  if(n != strlen(expect)){
    printf(1, "iouring_async_test: FAIL readback n=%d\n", n);
    exit();
  }
  for(i = 0; i < strlen(expect); i++){
    if(out[i] != expect[i]){
      printf(1, "iouring_async_test: FAIL contents\n");
      exit();
    }
  }

  printf(1, "iouring_async_test: PASS\n");
  exit();
}
