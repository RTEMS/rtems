/*
 *  This file contains the RTEMS implementation of the POSIX API
 *  routines tcgetattr and tcsetattr.
 *
 *  $Id$
 *
 */

#include <rtems.h>
#if defined(RTEMS_NEWLIB)

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <termios.h>

#include "internal.h"
#include "libio.h"

/*
 * tcgetattr/tcsetattr -- get/set attributes of a device.
 *
 *  submitted by K.Shibuya
 */

int
tcgetattr(int fd, struct termios *tp)
{
    return __rtems_ioctl(fd,RTEMS_IO_GET_ATTRIBUTES,tp);
}

int
tcsetattr(int fd, int opt, struct termios *tp)
{
    if(opt != TCSANOW)
      return -1;
    return __rtems_ioctl(fd,RTEMS_IO_SET_ATTRIBUTES,tp);
}

int
tcdrain(int fd)
{
	return __rtems_ioctl(fd,RTEMS_IO_TCDRAIN,0);
}

#endif
