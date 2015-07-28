/**
 * @file
 *
 * @brief Remove a Named Semaphore
 * @ingroup POSIX_SEMAPHORE
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

#include <stdarg.h>

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/posix/semaphoreimpl.h>
#include <rtems/seterr.h>

int sem_unlink(
  const char *name
)
{
  int                      status;
  POSIX_Semaphore_Control *the_semaphore;
  Objects_Id               the_semaphore_id;
  size_t                   name_len;

  _Objects_Allocator_lock();
  _Thread_Disable_dispatch();

  status = _POSIX_Semaphore_Name_to_id( name, &the_semaphore_id, &name_len );
  if ( status != 0 ) {
    _Thread_Enable_dispatch();
    _Objects_Allocator_unlock();
    rtems_set_errno_and_return_minus_one( status );
  }

  the_semaphore = (POSIX_Semaphore_Control *) _Objects_Get_local_object(
    &_POSIX_Semaphore_Information,
    _Objects_Get_index( the_semaphore_id )
  );

  the_semaphore->linked = false;
  _POSIX_Semaphore_Namespace_remove( the_semaphore );
  _POSIX_Semaphore_Delete( the_semaphore );

  _Thread_Enable_dispatch();
  _Objects_Allocator_unlock();

  return 0;
}
