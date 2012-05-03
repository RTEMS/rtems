/*
 *  sbrk.c
 *
 *  If the BSP wants to dynamically allocate the memory for the
 *  C Library heap (malloc) and/or be able to extend the heap,
 *  then this routine must be functional.  This is the default
 *  implementation which raises an error.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>

#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void * sbrk(ptrdiff_t incr)
{
  errno = ENOMEM;
  return (void *)-1;
}
