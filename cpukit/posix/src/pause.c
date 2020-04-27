/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Suspend Process Execution
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
#include <unistd.h>

/**
 * 3.4.2 Suspend Process Execution, P1003.1b-1993, p. 81
 */
int pause( void )
{
  sigset_t  all_signals;
  int       status;

  (void) sigfillset( &all_signals );

  status = sigtimedwait( &all_signals, NULL, NULL );

  return status;
}
