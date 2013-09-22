/**
 * @file
 *
 * @brief execve()
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

/*
 *  Needed to get the prototype for this newlib helper method
 */
#define _COMPILING_NEWLIB

#include <errno.h>
#include <rtems/seterr.h>
#include <sys/unistd.h>

int _execve(
  const char *path __attribute__((unused)),
  char *const argv[] __attribute__((unused)),
  char *const envp[] __attribute__((unused))
)
{
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
