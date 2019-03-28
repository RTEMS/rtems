/**
 * @file
 *
 * @ingroup ClassicBarrierImpl
 *
 * @brief Classic Barrier Manager Implementation
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

#include <rtems/rtems/barrierdata.h>
#include <rtems/score/corebarrierimpl.h>
#include <rtems/score/objectimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicBarrierImpl Classic Barrier Implementation
 *
 *  @ingroup RTEMSInternalClassic
 *
 *  @{
 */

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
  Objects_Id            id,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Context_initialize( queue_context );
  return (Barrier_Control *) _Objects_Get(
    id,
    &queue_context->Lock_context.Lock_context,
    &_Barrier_Information
  );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
