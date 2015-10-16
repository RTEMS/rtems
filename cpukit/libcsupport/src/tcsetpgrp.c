/**
 *  @file
 *
 *  @brief Set Foreground Process Group ID
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#if defined(RTEMS_NEWLIB)

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <termios.h>
/* #include <sys/ioctl.h> */

#include <rtems/libio.h>

/**
 *  POSIX 1003.1b 7.2.4 - Set Foreground Process Group ID
 */
int tcsetpgrp(
  int fd RTEMS_UNUSED,
  pid_t pid RTEMS_UNUSED )
{
  return 0;
}

#endif
