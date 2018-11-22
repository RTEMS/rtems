/**
 * @file
 *
 * @brief Private Support Information for POSIX Shared Memory
 *
 */

/*
 * Copyright (c) 2016 Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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

RTEMS_INLINE_ROUTINE POSIX_Shm_Control *_POSIX_Shm_Allocate_unprotected( void )
{
  return (POSIX_Shm_Control *)
    _Objects_Allocate_unprotected( &_POSIX_Shm_Information );
}

/**
 * @brief POSIX Shared Memory Free
 *
 * This routine frees a shm control block.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Shm_Free (
  POSIX_Shm_Control *the_shm
)
{
  _Objects_Free( &_POSIX_Shm_Information, &the_shm->Object );
}

RTEMS_INLINE_ROUTINE POSIX_Shm_Control *_POSIX_Shm_Get_by_name(
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

RTEMS_INLINE_ROUTINE void _POSIX_Shm_Update_atime(
  POSIX_Shm_Control *shm
)
{
  struct timeval now;
  gettimeofday( &now, 0 );
  shm->atime = now.tv_sec;
}

RTEMS_INLINE_ROUTINE void _POSIX_Shm_Update_mtime_ctime(
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
