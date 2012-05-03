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

#include <time.h>

#ifndef _POSIX_TIMERS
#error "rtems is supposed to have timer_create"
#endif

int test( void );

int test( void )
{
  clockid_t        clock_id = 0;
  timer_t          timerid = 0;
  struct sigevent  evp;
  int              result;

  evp.sigev_notify = SIGEV_NONE;
  evp.sigev_notify = SIGEV_SIGNAL;
  evp.sigev_notify = SIGEV_THREAD;
  evp.sigev_signo = SIGALRM;
  evp.sigev_value.sival_int = 0;
  evp.sigev_value.sival_ptr = NULL;

  result = timer_create( clock_id, &evp, &timerid );

  return result;
}
