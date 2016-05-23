/**
 * @file
 *
 * @ingroup ClassicSem
 *
 * @brief Classic Semaphores Implementation
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_SEMIMPL_H
#define _RTEMS_RTEMS_SEMIMPL_H

#include <rtems/rtems/sem.h>
#include <rtems/score/coremuteximpl.h>
#include <rtems/score/coresemimpl.h>
#include <rtems/score/mrspimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
extern Objects_Information _Semaphore_Information;

/**
 *  @brief Allocates a semaphore control block from
 *  the inactive chain of free semaphore control blocks.
 *
 *  This function allocates a semaphore control block from
 *  the inactive chain of free semaphore control blocks.
 */
RTEMS_INLINE_ROUTINE Semaphore_Control *_Semaphore_Allocate( void )
{
  return (Semaphore_Control *) _Objects_Allocate( &_Semaphore_Information );
}

/**
 *  @brief Frees a semaphore control block to the
 *  inactive chain of free semaphore control blocks.
 *
 *  This routine frees a semaphore control block to the
 *  inactive chain of free semaphore control blocks.
 */
RTEMS_INLINE_ROUTINE void _Semaphore_Free (
  Semaphore_Control *the_semaphore
)
{
  _Objects_Free( &_Semaphore_Information, &the_semaphore->Object );
}

RTEMS_INLINE_ROUTINE Semaphore_Control *_Semaphore_Do_get(
  Objects_Id               id,
  Thread_queue_Context    *queue_context
#if defined(RTEMS_MULTIPROCESSING)
  ,
  Thread_queue_MP_callout  mp_callout
#endif
)
{
  _Thread_queue_Context_initialize( queue_context, mp_callout );
  return (Semaphore_Control *) _Objects_Get(
    id,
    &queue_context->Lock_context,
    &_Semaphore_Information
  );
}

#if defined(RTEMS_MULTIPROCESSING)
  #define _Semaphore_Get( id, queue_context, mp_callout ) \
    _Semaphore_Do_get( id, queue_context, mp_callout )
#else
  #define _Semaphore_Get( id, queue_context, mp_callout ) \
    _Semaphore_Do_get( id, queue_context )
#endif

#ifdef __cplusplus
}
#endif

#ifdef RTEMS_MULTIPROCESSING
#include <rtems/rtems/semmp.h>
#endif

#endif
/*  end of include file */
