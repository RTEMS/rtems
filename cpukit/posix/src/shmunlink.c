/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 */

/*
 * Copyright (c) 2016 Gedare Bloom.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
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
  if ( shm ) {
    _Objects_Namespace_remove_string(
      &_POSIX_Shm_Information,
      &shm->Object
    );

    if ( shm->reference_count == 0 ) {
      /* Only remove the shm object if no references exist to it. Otherwise,
       * the shm object will be freed later in _POSIX_Shm_Attempt_delete */
      _POSIX_Shm_Free( shm );
    }
  } else {
    switch ( obj_err ) {
      case OBJECTS_GET_BY_NAME_NAME_TOO_LONG:
        err = ENAMETOOLONG;
        break;

      case OBJECTS_GET_BY_NAME_INVALID_NAME:
      case OBJECTS_GET_BY_NAME_NO_OBJECT:
      default:
        err = ENOENT;
        break;
    }
  }

  _Objects_Allocator_unlock();

  if ( err != 0 )
    rtems_set_errno_and_return_minus_one( err );
  return 0;
}
