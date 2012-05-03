/*
 *  This test file is used to verify that the header files associated with
 *  invoking this function are correct.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>

/* FIXME: POSIX.1-2001 marks ualarm() as obsolete.
 * POSIX.1-2008 removes the specification of ualarm(). */

useconds_t _EXFUN(ualarm, (useconds_t __useconds, useconds_t __interval));
int test( void );

int test( void )
{
  useconds_t useconds;
  useconds_t interval;
  useconds_t result;

  useconds = 10;
  interval = 10;

  result = ualarm( useconds, interval );

  return (result == 0) ? 0 : -1;
}
