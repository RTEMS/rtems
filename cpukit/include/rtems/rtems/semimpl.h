/**
 * @file
 *
 * @ingroup RTEMSImplClassicSemaphore
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicSemaphore.
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

#include <rtems/rtems/semdata.h>
#include <rtems/score/coremuteximpl.h>
#include <rtems/score/coresemimpl.h>
#include <rtems/score/mrspimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicSemaphore Semaphore Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Semaphore Manager implementation.
 *
 * @{
 */

/**
 * @brief Classic semaphore variants.
 *
 * Must be in synchronization with Semaphore_Control::variant.
 */
typedef enum {
  SEMAPHORE_VARIANT_MUTEX_INHERIT_PRIORITY,
  SEMAPHORE_VARIANT_MUTEX_PRIORITY_CEILING,
  SEMAPHORE_VARIANT_MUTEX_NO_PROTOCOL,
  SEMAPHORE_VARIANT_SIMPLE_BINARY,
  SEMAPHORE_VARIANT_COUNTING
#if defined(RTEMS_SMP)
  ,
  SEMAPHORE_VARIANT_MRSP
#endif
} Semaphore_Variant;

typedef enum {
  SEMAPHORE_DISCIPLINE_PRIORITY,
  SEMAPHORE_DISCIPLINE_FIFO
} Semaphore_Discipline;

RTEMS_INLINE_ROUTINE uintptr_t _Semaphore_Get_flags(
  const Semaphore_Control *the_semaphore
)
{
  _Assert( _Chain_Is_node_off_chain( &the_semaphore->Object.Node ) );
  return (uintptr_t) the_semaphore->Object.Node.previous;
}

RTEMS_INLINE_ROUTINE void _Semaphore_Set_flags(
  Semaphore_Control *the_semaphore,
  uintptr_t          flags
)
{
  _Assert( _Chain_Is_node_off_chain( &the_semaphore->Object.Node ) );
  the_semaphore->Object.Node.previous = (Chain_Node *) flags;
}

RTEMS_INLINE_ROUTINE Semaphore_Variant _Semaphore_Get_variant(
  uintptr_t flags
)
{
  return (Semaphore_Discipline) ( flags & 0x7 );
}

RTEMS_INLINE_ROUTINE uintptr_t _Semaphore_Set_variant(
  uintptr_t         flags,
  Semaphore_Variant variant
)
{
  return flags | variant;
}

RTEMS_INLINE_ROUTINE Semaphore_Discipline _Semaphore_Get_discipline(
  uintptr_t flags
)
{
  return (Semaphore_Discipline) ( ( flags >> 3 ) & 0x1 );
}

RTEMS_INLINE_ROUTINE uintptr_t _Semaphore_Set_discipline(
  uintptr_t            flags,
  Semaphore_Discipline discipline
)
{
  return flags | ( discipline << 3 );
}

#if defined(RTEMS_MULTIPROCESSING)
RTEMS_INLINE_ROUTINE bool _Semaphore_Is_global(
  uintptr_t flags
)
{
  return ( flags & 0x10 ) != 0;
}

RTEMS_INLINE_ROUTINE uintptr_t _Semaphore_Make_global( uintptr_t flags )
{
  return flags | 0x10;
}
#endif

RTEMS_INLINE_ROUTINE const Thread_queue_Operations *_Semaphore_Get_operations(
  uintptr_t flags
)
{
  if (
    _Semaphore_Get_variant( flags ) == SEMAPHORE_VARIANT_MUTEX_INHERIT_PRIORITY
  ) {
    return &_Thread_queue_Operations_priority_inherit;
  }

  if ( _Semaphore_Get_discipline( flags ) == SEMAPHORE_DISCIPLINE_PRIORITY ) {
    return &_Thread_queue_Operations_priority;
  }

  return &_Thread_queue_Operations_FIFO;
}

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

RTEMS_INLINE_ROUTINE Semaphore_Control *_Semaphore_Get(
  Objects_Id            id,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Context_initialize( queue_context );
  return (Semaphore_Control *) _Objects_Get(
    id,
    &queue_context->Lock_context.Lock_context,
    &_Semaphore_Information
  );
}

/** @} */

#ifdef __cplusplus
}
#endif

#ifdef RTEMS_MULTIPROCESSING
#include <rtems/rtems/semmp.h>
#endif

#endif
/*  end of include file */
