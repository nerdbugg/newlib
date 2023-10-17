/* carrier sycall wrapper */

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

extern int errno;

/* Register name faking - works in collusion with the linker.  */
register char * stack_ptr asm ("sp");

// note: used in trampoline code to issue hostcall
// in hybrid mode, the real address is computed relative to ddc
// in pure mode, we need runtime relocation to get real address
// (dirty hack:specify a hard coded length of array to distinguish it from other data reloc)
unsigned long local_cap_store = 0xe001000;

// note: defined in tramps.S
extern unsigned long c_out_1(unsigned long call_num, unsigned long arg1);
extern unsigned long c_out_2(unsigned long call_num, unsigned long arg1, unsigned long arg2);
extern unsigned long c_out_3(unsigned long call_num, unsigned long arg1, unsigned long arg2, 
			     unsigned long arg3);
extern unsigned long ret_from_monitor();
extern void copy_from_cap(void *dst, void *src_cap_location, int len);

unsigned long __hostcall(unsigned long call_num, unsigned long arg1, unsigned long arg2, 
				unsigned long arg3) {
  return c_out_3(call_num, arg1, arg2, arg3);
}

enum HC_NUM {
  OUTPUT = 1,
  EXIT = 13,
  NANOSLEEP = 200,
  SAVE = 115,
  OPEN_TAP = 300,
  PIPE = 301,
  SET_NOBLOCK = 302,
  MAKE_CALL = 403,
  CF_ADV = 405,
  CF_PRB = 406,
  CAP_WAIT = 407,
  CAP_WAKE = 408,
  CS_ADV = 409,
  CS_PRB = 410,
  MAKE_CAP_CALL = 411,
  FIN_CAP_CALL = 412,
  FETCH_CAP_CALL = 413,

  SOCKET = 500,
  SETSOCKOPT = 501,
  IOCTL = 502,
  ACCEPT4 = 503,
  LISTEN = 504,
  ACCEPT = 505,
  BIND = 506,
  SEND = 509,
  RECV = 510,
  SOCKETPAIR = 512,
  POLL= 513,
  SELECT = 514,
  RECVFROM = 519,
  WRITEV = 520,
  GETADDRINFO = 530,
  GETPEEREID = 531,
  CONNECT = 532,
  HOST_GET_MY_INNER = 700,
  HOST_SYSCALL_HANDLER_PRB = 701,
  HOST_GET_SC_CAPS = 702,
  GETTIMEOFDAY = 800,
  LSTAT = 801,
  STAT = 806,
  FSTAT = 807,
  UNLINK = 802,
  CLOSE = 803,
  ACCESS = 804,
  TRUNCATE = 808,
  READ = 809,
  WRITE = 810,
  OPEN = 811,
  LSEEK = 812,
  ERRNO = 813,
  FCNTL = 814,
};

long __syscall_ret(long r) {
  if(r<0) {
    errno = -r;
    return -1;
  }

  errno = 0;
  return r;
}

void _exit() {
  __hostcall(EXIT, 0, 0, 0);
}

int close(int file) {
  return __hostcall(CLOSE, file, 0, 0);
}

char *__env[1] = { 0 };
char **environ = __env;

int execve(char *name, char **argv, char **env) {
  errno = ENOMEM;
  return -1;
}

int fork(void) {
  errno = EAGAIN;
  return -1;
}

int fstat(int file, struct stat *st) {
  st->st_mode = S_IFCHR;
  return 0;
}

int getpid(void) {
  return 1;
}

int isatty(int file) {
  return 1;
}

int kill(int pid, int sig) {
  errno = EINVAL;
  return -1;
}

int link(char *old, char *new) {
  errno = EMLINK;
  return -1;
}

int lseek(int file, int ptr, int dir) {
  return 0;
}

int open(const char *name, int flags, ...) {
  mode_t mode = 0;

  if((flags & O_CREAT)) {
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, mode_t);
    va_end(ap);
  }

  int fd = __hostcall(OPEN, (unsigned long)name, flags, mode);

  return __syscall_ret(fd);
}

int read(int file, char *ptr, int len) {
  return __hostcall(READ, file, (unsigned long)ptr, len);
  return 0;
}

int write(int file, char *ptr, int len) {
  return __hostcall(WRITE, file, (unsigned long)ptr, len);
}

caddr_t sbrk(int incr) {
  extern char _end;		/* Defined by the linker */
  static char *heap_end;
  char *prev_heap_end;
 
  if (heap_end == 0) {
    heap_end = &_end;
  }
  prev_heap_end = heap_end;
  if (heap_end + incr > stack_ptr) {
    write (1, "Heap and stack collision\n", 25);
    _exit();
  }

  heap_end += incr;
  return (caddr_t) prev_heap_end;
}

int stat(const char *__restrict __path, struct stat *__restrict __sbuf) {
  __sbuf->st_mode = S_IFCHR;
  return 0;
}

clock_t times(struct tms *buf) {
  return -1;
}

int unlink(char *name) {
  errno = ENOENT;
  return -1; 
}

int wait(int *status) {
  errno = ECHILD;
  return -1;
}

