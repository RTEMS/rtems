/**
 *  @file
 *
 *  @brief Get Foreground Process Group ID
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE_TCGETPGRP)

#include <sys/types.h>
#include <unistd.h>

/**
 *  POSIX 1003.1b 7.2.3 - Get Foreground Process Group ID
 */
pid_t tcgetpgrp(int fd)
{
  (void) fd;
  return getpid();
}

#endif
