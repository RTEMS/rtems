/**
 * @file
 */

/*
 * Copyright (c) 2016 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/mman.h>
#include <errno.h>
#include <rtems/seterr.h>

#include <rtems/posix/shmimpl.h>

int shm_unlink( const char *name )
{
  Objects_Get_by_name_error obj_err;
  int err = 0;
  POSIX_Shm_Control *shm;

  _Objects_Allocator_lock();

  shm = _POSIX_Shm_Get_by_name( name, 0, &obj_err );
  switch ( obj_err ) {
    case OBJECTS_GET_BY_NAME_INVALID_NAME:
      err = ENOENT;
      break;

    case OBJECTS_GET_BY_NAME_NAME_TOO_LONG:
      err = ENAMETOOLONG;
      break;

    case OBJECTS_GET_BY_NAME_NO_OBJECT:
    default:
      _Objects_Namespace_remove_string(
        &_POSIX_Shm_Information,
        &shm->Object
      );

      if ( shm->reference_count == 0 ) {
        /* Only remove the shm object if no references exist to it. Otherwise,
         * the shm object will be freed later in _POSIX_Shm_Attempt_delete */
        _POSIX_Shm_Free( shm );
      }
      break;
  }

  _Objects_Allocator_unlock();

  if ( err != 0 )
    rtems_set_errno_and_return_minus_one( err );
  return 0;
}
