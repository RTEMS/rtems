#if !defined(RTEMS_UNIX)

/*
 *  RTEMS "Broken" __brk/__sbrk Implementation
 *
 *  NOTE: sbrk is BSP provided.
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems.h>

#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#ifdef RTEMS_NEWLIB
#include <reent.h>
#endif
#include <unistd.h>

/* we use RTEMS for memory management.  We don't need sbrk */

void * __sbrk(int incr)
{
  errno = EINVAL;
  return (void *)0;
}

int __brk( const void *endds )
{
  errno = EINVAL;
  return -1;
}

#endif
