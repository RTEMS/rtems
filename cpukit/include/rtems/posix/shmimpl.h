/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Private Support Information for POSIX Shared Memory
 *
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

#ifndef _RTEMS_POSIX_SHMIMPL_H
#define _RTEMS_POSIX_SHMIMPL_H

#include <rtems/fs.h>
#include <rtems/libio.h>
#include <rtems/posix/posixapi.h>
#include <rtems/posix/shm.h>
#include <rtems/score/objectimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup POSIXShmPrivate
 * @{
 */

static inline POSIX_Shm_Control *_POSIX_Shm_Allocate_unprotected( void )
{
  return (POSIX_Shm_Control *)
    _Objects_Allocate_unprotected( &_POSIX_Shm_Information );
}

/**
 * @brief POSIX Shared Memory Free
 *
 * This routine frees a shm control block.
 */
static inline void _POSIX_Shm_Free (
  POSIX_Shm_Control *the_shm
)
{
  _Objects_Free( &_POSIX_Shm_Information, &the_shm->Object );
}

static inline POSIX_Shm_Control *_POSIX_Shm_Get_by_name(
  const char                *name,
  size_t                    *name_length_p,
  Objects_Get_by_name_error *error
)
{
  return (POSIX_Shm_Control *) _Objects_Get_by_name(
    &_POSIX_Shm_Information,
    name,
    name_length_p,
    error
  );
}

static inline void _POSIX_Shm_Update_atime(
  POSIX_Shm_Control *shm
)
{
  struct timeval now;
  gettimeofday( &now, 0 );
  shm->atime = now.tv_sec;
}

static inline void _POSIX_Shm_Update_mtime_ctime(
  POSIX_Shm_Control *shm
)
{
  struct timeval now;
  gettimeofday( &now, 0 );
  shm->mtime = now.tv_sec;
  shm->ctime = now.tv_sec;
}

static inline POSIX_Shm_Control* iop_to_shm( rtems_libio_t *iop )
{
  return (POSIX_Shm_Control*) iop->data1;
}

static inline POSIX_Shm_Control* loc_to_shm(
    const rtems_filesystem_location_info_t *loc
)
{
  return (POSIX_Shm_Control*) loc->node_access;
}

static inline int POSIX_Shm_Attempt_delete(
    POSIX_Shm_Control* shm
)
{
  Objects_Control       *obj;
  ISR_lock_Context       lock_ctx;
  int err;

  err = 0;

  _Objects_Allocator_lock();
  --shm->reference_count;
  if ( shm->reference_count == 0 ) {
    if ( (*shm->shm_object.ops->object_delete)( &shm->shm_object ) != 0 ) {
      err = EIO;
    }
  }
  /* check if the object has been unlinked yet. */
  obj = _Objects_Get( shm->Object.id, &lock_ctx, &_POSIX_Shm_Information );
  if ( obj == NULL ) {
    /* if it was unlinked, then it can be freed. */
    _POSIX_Shm_Free( shm );
  } else {
    /* it will be freed when it is unlinked. */
    _ISR_lock_ISR_enable( &lock_ctx );
  }
  _Objects_Allocator_unlock();
  return err;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
