/*
 *  This file contains the RTEMS implementation of the POSIX API
 *  routines cfgetispeed, cfgetospeed, cfsetispeed and cfsetospeed.
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

speed_t
cfgetospeed(const struct termios *tp)
{
	return tp->c_cflag & CBAUD;
}

int
cfsetospeed(struct termios *tp, speed_t speed)
{
	if (speed & ~CBAUD) {
		errno = EINVAL;
		return -1;
	}
	tp->c_cflag = (tp->c_cflag & ~CBAUD) | speed;
	return 0;
}

speed_t
cfgetispeed(const struct termios *tp)
{
	return (tp->c_cflag / (CIBAUD / CBAUD)) &  CBAUD;
}

int
cfsetispeed(struct termios *tp, speed_t speed)
{
	if (speed & ~CBAUD) {
		errno = EINVAL;
		return -1;
	}
	tp->c_cflag = (tp->c_cflag & ~CBAUD) | (speed * (CIBAUD / CBAUD));
	return 0;
}

#endif
