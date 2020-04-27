/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief _execve()
 *
 * The Newlib C Library contains all of the exec*() variants and assumes
 * the underlying OS support provides _execve(). This single method
 * ensures that all exec*() variants return ENOSYS.
 */

/*
 *  COPYRIGHT (c) 1989-2013,2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
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
  const char *path,
  char *const argv[],
  char *const envp[]
)
{
  (void) path;
  (void) argv;
  (void) envp;
  rtems_set_errno_and_return_minus_one( ENOSYS );
}
