/*
 *  This file contains the RTEMS implementation of the POSIX API
 *  routines tcdrain.
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

int
tcdrain(int fd)
{
	return __rtems_ioctl(fd,RTEMS_IO_TCDRAIN,0);
}

#endif
