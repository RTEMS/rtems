/*
 *  sbrk.c
 *
 *  If the BSP wants to dynamically allocate the memory for the
 *  C Library heap (malloc) and/or be able to extend the heap,
 *  then this routine must be functional.  This is the default
 *  implementation which raises an error.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994, 1997.
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
#include <unistd.h>

void * sbrk(size_t incr)
{
  errno = ENOMEM;
  return (void *)-1;
}

