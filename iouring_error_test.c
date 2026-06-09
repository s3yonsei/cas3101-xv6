#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "iouring.h"

#define TEST_PGSIZE 4096

char bigbuf[TEST_PGSIZE + 1];

int
main(void)
{
  int fd, i;
  char small[8];
  struct io_uring ring;
  struct io_uring fullring;
  struct io_uring_cqe cqe;

  unlink("ior_error");
  fd = open("ior_error", O_CREATE | O_RDWR);
  if(fd < 0 || iouring_setup(&ring) < 0){
    printf(1, "iouring_error_test: FAIL setup\n");
    exit();
  }
  if(ring.entries != IORING_MAX_ENTRIES){
    printf(1, "iouring_error_test: FAIL fixed entries=%d\n", ring.entries);
    exit();
  }

  memset(&fullring, 0, sizeof(fullring));
  fullring.entries = IORING_MAX_ENTRIES;
  fullring.sq_tail = IORING_MAX_ENTRIES;
  if(io_uring_prep(&fullring, IORING_OP_WRITE, fd,
                   small, sizeof(small), 99) >= 0 ||
     fullring.sq_tail != IORING_MAX_ENTRIES){
    printf(1, "iouring_error_test: FAIL full sq tail=%d\n",
           fullring.sq_tail);
    exit();
  }

  if(io_uring_prep(&ring, IORING_OP_READ, -1, small, sizeof(small), 1) < 0 ||
     io_uring_prep(&ring, IORING_OP_WRITE, fd, bigbuf, sizeof(bigbuf), 2) < 0){
    printf(1, "iouring_error_test: FAIL prep\n");
    exit();
  }

  for(i = 0; i < 2; i++){
    if(io_uring_wait_cqe(&ring, &cqe) < 0 || cqe.res != -1){
      printf(1, "iouring_error_test: FAIL cqe\n");
      exit();
    }
  }

  iouring_close(&ring);
  close(fd);
  printf(1, "iouring_error_test: PASS\n");
  exit();
}
