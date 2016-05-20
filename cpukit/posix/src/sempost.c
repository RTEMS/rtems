/**
 *  @file
 *
 *  @brief Unlock a Semaphore
 *  @ingroup POSIX_SEMAPHORE
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <semaphore.h>
#include <limits.h>

#include <rtems/posix/semaphoreimpl.h>
#include <rtems/posix/posixapi.h>

int sem_post(
  sem_t  *sem
)
{
  POSIX_Semaphore_Control *the_semaphore;
  Thread_queue_Context     queue_context;
  Status_Control           status;

  the_semaphore = _POSIX_Semaphore_Get( sem, &queue_context );

  if ( the_semaphore == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  status = _CORE_semaphore_Surrender(
    &the_semaphore->Semaphore,
    POSIX_SEMAPHORE_TQ_OPERATIONS,
    SEM_VALUE_MAX,
    &queue_context
  );
  return _POSIX_Zero_or_minus_one_plus_errno( status );
}
