/**
 * @file rtems/rtems/barrier.inl
 *
 * @defgroup ClassicBarrier Barriers
 *
 * @ingroup ClassicRTEMS
 * @brief Inline Implementation from Barrier Manager
 *
 * This file contains the static inlin implementation of the inlined
 * routines from the Barrier Manager.
 */

/*
 * COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_BARRIERIMPL_H
#define _RTEMS_RTEMS_BARRIERIMPL_H

#include <rtems/rtems/barrier.h>
#include <rtems/score/corebarrierimpl.h>
#include <rtems/score/objectimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicBarrierImpl Classic Barrier Implementation
 *
 *  @ingroup ClassicBarrier
 *
 *  @{
 */

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
extern Objects_Information _Barrier_Information;

/**
 *  @brief _Barrier_Allocate
 *
 *  This function allocates a barrier control block from
 *  the inactive chain of free barrier control blocks.
 */
RTEMS_INLINE_ROUTINE Barrier_Control *_Barrier_Allocate( void )
{
  return (Barrier_Control *) _Objects_Allocate( &_Barrier_Information );
}

/**
 *  @brief _Barrier_Free
 *
 *  This routine frees a barrier control block to the
 *  inactive chain of free barrier control blocks.
 */
RTEMS_INLINE_ROUTINE void _Barrier_Free (
  Barrier_Control *the_barrier
)
{
  _CORE_barrier_Destroy( &the_barrier->Barrier );
  _Objects_Free( &_Barrier_Information, &the_barrier->Object );
}

RTEMS_INLINE_ROUTINE Barrier_Control *_Barrier_Get(
  Objects_Id        id,
  ISR_lock_Context *lock_context
)
{
  return (Barrier_Control *)
    _Objects_Get_local( id, &_Barrier_Information, lock_context );
}

/**
 * @brief Translate SuperCore Barrier Status Code to RTEMS Status Code
 *
 * This function returns a RTEMS status code based on the barrier
 * status code specified.
 *
 * @param[in] the_status is the SuperCore Barrier status to translate.
 *
 * @retval a status code indicating success or the reason for failure.
 */
rtems_status_code _Barrier_Translate_core_barrier_return_code (
  CORE_barrier_Status  the_status
);

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
