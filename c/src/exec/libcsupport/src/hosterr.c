/*
 *  Routines to access a host errno
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
#include <errno.h>

int host_errno(void);

/*
 * copy host errno, if any to thread aware errno, if any
 */

void fix_syscall_errno(void)
{
    errno = host_errno();
}

/*
 * Get the host system errno, if any
 * When using newlib (or possibly other libc's) on top of UNIX
 * the errno returned by system calls may be unavailable due
 * to trickery of making errno thread aware.
 * This provides a kludge of getting at it.
 */

#undef errno
extern int errno;
int host_errno(void)
{
    return errno;
}

