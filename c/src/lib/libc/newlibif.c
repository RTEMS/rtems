
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

int
read(int fd,
     char *buf,
     int nbytes)
{
    return __rtems_read(fd, buf, nbytes);
}

int
write(int fd,
      char *buf,
      int nbytes)
{
    return __rtems_write(fd, buf, nbytes);
}

int
open(char *buf,
     int flags,
     int mode)
{
    return __rtems_open(buf, flags, mode);
}

int
close(int fd)
{
    return __rtems_close(fd);
}

/*
 * lseek -- move read/write pointer. Since a serial port
 *          is non-seekable, we return an error.
 */
off_t
lseek(int fd,
      off_t offset,
      int whence)
{
    return __rtems_lseek(fd, offset, whence);
}

/*
 * fstat -- get status of a file. Since we have no file
 *          system, we just return an error.
 */
int
fstat(int fd,
      struct stat *buf)
{
    return __rtems_fstat(fd, buf);
}

/*
 *  getpid and kill are provided directly by rtems
 */

/*
 * ioctl -- IO control
 */

int
ioctl(int fd, int request, void *argp)
{
    return __rtems_ioctl(fd,request,argp);
}

#endif
