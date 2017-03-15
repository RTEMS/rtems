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

extern Objects_Information _POSIX_Shm_Information;

RTEMS_INLINE_ROUTINE void _POSIX_Shm_Acquire(
  POSIX_Shm_Control                 *the_shm,
  Thread_queue_Context              *queue_context
)
{
  _Thread_queue_Acquire(
    &the_shm->Wait_queue,
    queue_context
  );
}

RTEMS_INLINE_ROUTINE void _POSIX_Shm_Release(
  POSIX_Shm_Control                 *the_shm,
  Thread_queue_Context              *queue_context
)
{
  _Thread_queue_Release( &the_shm->Wait_queue, queue_context );
}

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

/** @} */

#ifdef __cplusplus
}
#endif

#endif
