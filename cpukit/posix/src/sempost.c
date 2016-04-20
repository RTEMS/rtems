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

#include <rtems/posix/semaphoreimpl.h>

int sem_post(
  sem_t  *sem
)
{
  POSIX_Semaphore_Control *the_semaphore;
  ISR_lock_Context         lock_context;

  the_semaphore = _POSIX_Semaphore_Get( sem, &lock_context );

  if ( the_semaphore == NULL ) {
    rtems_set_errno_and_return_minus_one( EINVAL );
  }

  _CORE_semaphore_Surrender(
    &the_semaphore->Semaphore,
    NULL,
    0,
    &lock_context
  );
  return 0;
}
