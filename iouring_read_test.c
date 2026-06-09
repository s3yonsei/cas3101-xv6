#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "iouring.h"

int
main(void)
{
  int fd;
  int i;
  char buf[32];
  char data[] = "async-read";
  struct io_uring ring;
  struct io_uring_cqe cqe;

  unlink("ior_read");
  fd = open("ior_read", O_CREATE | O_RDWR);
  if(fd < 0){
    printf(1, "iouring_read_test: FAIL open create\n");
    exit();
  }
  write(fd, data, strlen(data));
  close(fd);

  fd = open("ior_read", O_RDONLY);
  if(fd < 0 || iouring_setup(&ring) < 0){
    printf(1, "iouring_read_test: FAIL setup\n");
    exit();
  }

  memset(buf, 0, sizeof(buf));
  if(io_uring_prep(&ring, IORING_OP_READ, fd, buf, strlen(data), 11) < 0){
    printf(1, "iouring_read_test: FAIL prep\n");
    exit();
  }
  if(io_uring_wait_cqe(&ring, &cqe) < 0){
    printf(1, "iouring_read_test: FAIL wait\n");
    exit();
  }

  if(cqe.tag != 11 || cqe.res != strlen(data)){
    printf(1, "iouring_read_test: FAIL cqe res=%d tag=%d\n",
           cqe.res, cqe.tag);
    exit();
  }
  for(i = 0; i < strlen(data); i++){
    if(buf[i] != data[i]){
      printf(1, "iouring_read_test: FAIL buf\n");
      exit();
    }
  }

  iouring_close(&ring);
  close(fd);
  printf(1, "iouring_read_test: PASS\n");
  exit();
}
