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

#ifndef _POSIX_CPUTIME
#error "rtems is supposed to have clock_setenable_attr"
#endif
#ifndef _POSIX_THREAD_CPUTIME
#error "rtems is supposed to have clock_setenable_attr"
#endif

int test( void );

int test( void )
{
  clockid_t  clock_id = 0;
  int        attr;
  int        result;

  attr = CLOCK_ENABLED;
  attr = CLOCK_DISABLED;

  result = clock_setenable_attr( clock_id, attr );

  return result;
}
