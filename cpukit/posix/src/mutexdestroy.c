/**
 * @file
 *
 * @brief Initializing and Destroying a Mutex
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/muteximpl.h>

/*
 *  11.3.2 Initializing and Destroying a Mutex, P1003.1c/Draft 10, p. 87
 */

int pthread_mutex_destroy(
  pthread_mutex_t           *mutex
)
{
  POSIX_Mutex_Control  *the_mutex;
  unsigned long         flags;
  Thread_queue_Context  queue_context;
  int                   eno;

  the_mutex = _POSIX_Mutex_Get( mutex );
  POSIX_MUTEX_VALIDATE_OBJECT( the_mutex, flags );

  _POSIX_Mutex_Acquire( the_mutex, &queue_context );

  if ( _POSIX_Mutex_Get_owner( the_mutex ) == NULL ) {
    the_mutex->flags = ~the_mutex->flags;
    eno = 0;
  } else {
    eno = EBUSY;
  }

  _POSIX_Mutex_Release( the_mutex, &queue_context );
  return eno;
}
