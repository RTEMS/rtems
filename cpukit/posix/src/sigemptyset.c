/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Manipulate Signal Sets
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>

#include <rtems/posix/sigset.h>
#include <rtems/seterr.h>

int sigemptyset(
  sigset_t   *set
)
{
  if ( !set )
    rtems_set_errno_and_return_minus_one( EINVAL );

  *set = 0;
  return 0;
}
