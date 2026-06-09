#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "iouring.h"

int
main(void)
{
  int fd, n, i;
  char out[32];
  char data[] = "async-write";
  struct io_uring ring;
  struct io_uring_cqe cqe;

  unlink("ior_write");
  fd = open("ior_write", O_CREATE | O_RDWR);
  if(fd < 0 || iouring_setup(&ring) < 0){
    printf(1, "iouring_write_test: FAIL setup\n");
    exit();
  }

  if(io_uring_prep(&ring, IORING_OP_WRITE, fd, data, strlen(data), 22) < 0){
    printf(1, "iouring_write_test: FAIL prep\n");
    exit();
  }
  if(io_uring_wait_cqe(&ring, &cqe) < 0){
    printf(1, "iouring_write_test: FAIL wait\n");
    exit();
  }
  if(cqe.tag != 22 || cqe.res != strlen(data)){
    printf(1, "iouring_write_test: FAIL cqe res=%d tag=%d\n",
           cqe.res, cqe.tag);
    exit();
  }
  iouring_close(&ring);
  close(fd);

  fd = open("ior_write", O_RDONLY);
  memset(out, 0, sizeof(out));
  n = read(fd, out, sizeof(out));
  close(fd);
  if(n != strlen(data)){
    printf(1, "iouring_write_test: FAIL readback n=%d\n", n);
    exit();
  }
  for(i = 0; i < strlen(data); i++){
    if(out[i] != data[i]){
      printf(1, "iouring_write_test: FAIL contents\n");
      exit();
    }
  }

  printf(1, "iouring_write_test: PASS\n");
  exit();
}
