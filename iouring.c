#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "iouring.h"

#define IORING_OFFSET(field) ((uint)&(((struct io_uring*)0)->field))

struct iouring_state {
  int used;
  int closing;
  int entries;
  int pending;
  struct proc *owner;
  pde_t *pgdir;
  struct io_uring *user_ring;
};

struct iouring_req {
  int valid;
  int opcode;
  int len;
  uint buf;
  uint tag;
  struct file *file;
  struct iouring_state *ring;
};

static struct {
  struct spinlock lock;
  struct iouring_state rings[NPROC];
} iouring;

static void
iouring_worker(void *arg)
{
  (void)arg;
}

void
iouringinit(void)
{
  initlock(&iouring.lock, "iouring");
  kthread_create("iouring", iouring_worker, 0);
}

int
iouring_setup(struct io_uring *uring)
{
  return 0;
}

int
iouring_wait(struct io_uring *uring)
{
  return 0;
}

int
iouring_close(struct io_uring *uring)
{
  return 0;
}

int
sys_iouring_setup(void)
{
  char *ring;

  if(argptr(0, &ring, sizeof(struct io_uring)) < 0)
    return -1;
  return iouring_setup((struct io_uring*)ring);
}

int
sys_iouring_wait(void)
{
  char *ring;

  if(argptr(0, &ring, sizeof(struct io_uring)) < 0)
    return -1;
  return iouring_wait((struct io_uring*)ring);
}

int
sys_iouring_close(void)
{
  char *ring;

  if(argptr(0, &ring, sizeof(struct io_uring)) < 0)
    return -1;
  return iouring_close((struct io_uring*)ring);
}
