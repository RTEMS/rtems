/**
 * @file
 * 
 * @brief Inlined Routines from the POSIX Barrier Manager
 *
 * This file contains the static inlin implementation of the inlined
 * routines from the POSIX Barrier Manager.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_BARRIERIMPL_H
#define _RTEMS_POSIX_BARRIERIMPL_H

#include <rtems/posix/barrier.h>
#include <rtems/score/corebarrierimpl.h>
#include <rtems/score/objectimpl.h>

#include <errno.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

extern Objects_Information _POSIX_Barrier_Information;

/**
 * @brief POSIX translate barrier return code.
 * 
 * This routine translates SuperCore Barrier status codes into the
 * corresponding POSIX ones.
 *
 * @param[in] the_barrier_status is the SuperCore status.
 *
 * @return the corresponding POSIX status
 */
int _POSIX_Barrier_Translate_core_barrier_return_code(
  CORE_barrier_Status  the_barrier_status
);

/**
 * @brief Allocate a barrier control block.
 *
 * This function allocates a barrier control block from
 * the inactive chain of free barrier control blocks.
 */
RTEMS_INLINE_ROUTINE POSIX_Barrier_Control *_POSIX_Barrier_Allocate( void )
{
  return (POSIX_Barrier_Control *) 
    _Objects_Allocate( &_POSIX_Barrier_Information );
}

/**
 * @brief Free a barrier control block.
 *
 * This routine frees a barrier control block to the
 * inactive chain of free barrier control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Barrier_Free (
  POSIX_Barrier_Control *the_barrier
)
{
  _CORE_barrier_Destroy( &the_barrier->Barrier );
  _Objects_Free( &_POSIX_Barrier_Information, &the_barrier->Object );
}

RTEMS_INLINE_ROUTINE POSIX_Barrier_Control *_POSIX_Barrier_Get (
  const pthread_barrier_t *barrier,
  ISR_lock_Context        *lock_context
)
{
  return (POSIX_Barrier_Control *) _Objects_Get_local(
    (Objects_Id) *barrier,
    &_POSIX_Barrier_Information,
    lock_context
  );
}

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
