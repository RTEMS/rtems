/*
 *  Routines to access a host errno
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
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

