/*
 *  This test file is used to verify that the header files associated with
 *  invoking this function are correct.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <sched.h>

#ifndef _POSIX_PRIORITY_SCHEDULING
#error "rtems is supposed to have sched_getparam"
#endif
 
void test( void )
{
  pid_t  pid;
  struct sched_param param;
  int    result;

  pid = 0;

  result = sched_getparam( pid, &param );
}
