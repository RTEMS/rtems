
/*
 *  This file contains the glue which maps newlib system calls onto their
 *  RTEMS implementations.  This formerly was in the file
 *  libgloss/rtems/iface.c which was installed as rtems.o.  Merging this
 *  into the RTEMS source tree minimizes the files which must be linked
 *  to build an rtems application.
 *
 *  $Id$
 *
 */

#include <rtems.h>
#if defined(RTEMS_NEWLIB)

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <reent.h>

#include <termios.h>

#include "internal.h"
#include "libio.h"

_ssize_t _read_r(
  struct _reent *ptr,
  int fd,
  void *buf,
  size_t nbytes
)
{
  return __rtems_read(fd, buf, nbytes);
}

long _write_r(
  struct _reent *ptr,
  int fd,
  const void *buf,
  size_t nbytes
)
{
  return __rtems_write(fd, buf, nbytes);
}

int _open_r(
  struct _reent *ptr,
  const char *buf,
  int flags,
  int mode
)
{
  return __rtems_open(buf, flags, mode);
}

int _close_r(
  struct _reent *ptr,
  int fd
)
{
  return __rtems_close(fd);
}

off_t _lseek_r(
  struct _reent *ptr,
  int fd,
  off_t offset,
  int whence
)
{
  return __rtems_lseek(fd, offset, whence);
}

int _stat_r(
  struct _reent *ptr,
  const char *path,
  struct stat *buf
)
{
  return stat(path, buf);
}

int _fstat_r(
  struct _reent *ptr,
  int fd,
  struct stat *buf
)
{
  return __rtems_fstat(fd, buf);
}

pid_t _getpid_r(struct _reent *ptr)
{
  pid_t __getpid();
  return __getpid();
}

int _kill_r( struct _reent *ptr, pid_t pid, int sig )
{
  int __kill();
  return __kill(pid, sig);
}
#endif
