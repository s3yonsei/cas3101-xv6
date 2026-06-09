#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "iouring.h"

char*
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(const char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, const void *vsrc, int n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}

int
io_uring_prep(struct io_uring *ring, int opcode, int fd, void *buf,
              int len, uint tag)
{
  uint tail;
  struct io_uring_sqe *sqe;

  if (ring == 0) return -1;
  if (ring->sq_tail - ring->sq_head >= (uint)ring->entries) return -1;

  tail = ring->sq_tail;
  sqe = &ring->sqes[tail % ring->entries];
  memset(sqe, 0, sizeof(*sqe));
  sqe->fd = fd;
  sqe->buf = buf;
  sqe->len = len;
  sqe->tag = tag;
  sqe->opcode = opcode;

  // Publish SQE fields before advancing the producer tail.
  __sync_synchronize();
  ring->sq_tail = tail + 1;
  return 0;
}

int
io_uring_wait_cqe(struct io_uring *ring, struct io_uring_cqe *cqe)
{
  if (ring == 0 || cqe == 0) return -1;
  while (ring->cq_head == ring->cq_tail) {
    if (iouring_wait(ring) < 0) return -1;
  }
  
  __sync_synchronize();
  *cqe = ring->cqes[ring->cq_head % ring->entries];
  __sync_synchronize();
  ring->cq_head++;
  return 0;
}
