/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Synchronously Accept a Signal
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

#include <stddef.h>
#include <signal.h>
#include <errno.h>

int sigwait(
  const sigset_t  *__restrict set,
  int             *__restrict sig
)
{
  int status;

  status = sigtimedwait( set, NULL, NULL );

  if ( status != -1 ) {
    if ( sig )
      *sig = status;
    return 0;
  }

  return errno;
}
