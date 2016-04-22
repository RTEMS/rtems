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

#include <rtems/posix/condimpl.h>

static bool _POSIX_Condition_variables_Check_id_and_auto_init(
  pthread_cond_t *cond
)
{
  if ( cond == NULL ) {
    return false;
  }

  if ( *cond == PTHREAD_COND_INITIALIZER ) {
    int eno;

    _Once_Lock();

    if ( *cond == PTHREAD_COND_INITIALIZER ) {
      eno = pthread_cond_init( cond, NULL );
    } else {
      eno = 0;
    }

    _Once_Unlock();

    if ( eno != 0 ) {
      return false;
    }
  }

  return true;
}

POSIX_Condition_variables_Control *_POSIX_Condition_variables_Get(
  pthread_cond_t   *cond,
  ISR_lock_Context *lock_context
)
{
  if ( !_POSIX_Condition_variables_Check_id_and_auto_init( cond ) ) {
    return NULL;
  }

  return (POSIX_Condition_variables_Control *) _Objects_Get_local(
    (Objects_Id) *cond,
    &_POSIX_Condition_variables_Information,
    lock_context
  );
}
