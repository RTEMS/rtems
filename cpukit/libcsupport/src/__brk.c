#if !defined(RTEMS_UNIX)

/*
 *  RTEMS "Broken" __brk/__sbrk Implementation
 *
 *  NOTE: sbrk is BSP provided.
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
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
