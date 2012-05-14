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

int test( void );

int test( void )
{
  struct tm timestruct;
  time_t    time_encoded;

  timestruct.tm_sec = 0;
  timestruct.tm_min = 0;
  timestruct.tm_hour = 0;
  timestruct.tm_mday = 1;
  timestruct.tm_mon = 0;
  timestruct.tm_year = 0;
  timestruct.tm_wday = 0;
  timestruct.tm_yday = 0;
  timestruct.tm_isdst = 0;

  time_encoded = mktime( &timestruct );

  return (time_encoded != -1);
}
