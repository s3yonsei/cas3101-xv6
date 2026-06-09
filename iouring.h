#ifndef XV6_IOURING_H
#define XV6_IOURING_H

#define IORING_MAX_ENTRIES 16

#define IORING_OP_READ  1
#define IORING_OP_WRITE 2

struct io_uring_sqe {
  int opcode;       // I/O operation type
  int fd;           // target file descriptor
  char *buf;        // user buffer for read/write
  int len;          // number of bytes to transfer
  uint tag;         // user-defined request tag
};

struct io_uring_cqe {
  uint tag;         // copied from the completed SQE
  int res;          // result: bytes transferred or -1
};

struct io_uring {
  uint sq_head;     // next SQE index consumed by kernel
  uint sq_tail;     // next SQE index produced by user
  uint cq_head;     // next CQE index consumed by user
  uint cq_tail;     // next CQE index produced by kernel
  int entries;      // active ring size
  struct io_uring_sqe sqes[IORING_MAX_ENTRIES]; // submission entries
  struct io_uring_cqe cqes[IORING_MAX_ENTRIES]; // completion entries
};

#endif
