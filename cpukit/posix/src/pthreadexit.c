/*
 *  16.1.5.1 Thread Termination, p1003.1c/Draft 10, p. 150
 *
 *  NOTE: Key destructors are executed in the POSIX api delete extension.
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>
#include <assert.h>

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/posix/pthread.h>

void pthread_exit(
  void  *value_ptr
)
{
  Objects_Information     *the_information;

  the_information = _Objects_Get_information( _Thread_Executing->Object.id );

  /* This should never happen if _Thread_Get() works right */
  assert( the_information );

  _Thread_Disable_dispatch();

  _Thread_Executing->Wait.return_argument = (unsigned32 *)value_ptr;

  _Thread_Close( the_information, _Thread_Executing );

  _POSIX_Threads_Free( _Thread_Executing );

  _Thread_Enable_dispatch();
}

