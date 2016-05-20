/**
 * @file
 * 
 * @brief Private Inlined Routines for POSIX Semaphores
 *
 * This include file contains the static inline implementation of the private 
 * inlined routines for POSIX Semaphores.
 */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_SEMAPHOREIMPL_H
#define _RTEMS_POSIX_SEMAPHOREIMPL_H

#include <rtems/posix/semaphore.h>
#include <rtems/posix/posixapi.h>
#include <rtems/score/coresemimpl.h>
#include <rtems/seterr.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This defines the information control block used to manage
 *  this class of objects.
 */
extern Objects_Information _POSIX_Semaphore_Information;

#define POSIX_SEMAPHORE_TQ_OPERATIONS &_Thread_queue_Operations_FIFO

RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *
  _POSIX_Semaphore_Allocate_unprotected( void )
{
  return (POSIX_Semaphore_Control *)
    _Objects_Allocate_unprotected( &_POSIX_Semaphore_Information );
}

/**
 *  @brief POSIX Semaphore Free
 *
 *  This routine frees a semaphore control block to the
 *  inactive chain of free semaphore control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Semaphore_Free (
  POSIX_Semaphore_Control *the_semaphore
)
{
  _Objects_Free( &_POSIX_Semaphore_Information, &the_semaphore->Object );
}

RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *_POSIX_Semaphore_Get(
  const sem_t          *id,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Context_initialize( queue_context );
  return (POSIX_Semaphore_Control *) _Objects_Get(
    (Objects_Id) *id,
    &queue_context->Lock_context,
    &_POSIX_Semaphore_Information
  );
}

/**
 *  @brief POSIX Semaphore Create Support
 *
 *  This routine supports the sem_init and sem_open routines.
 */

int _POSIX_Semaphore_Create_support(
  const char                *name,
  size_t                     name_len,
  int                        pshared,
  unsigned int               value,
  POSIX_Semaphore_Control  **the_sem
);

/**
 *  @brief POSIX Semaphore Delete
 *
 * This routine supports the sem_close and sem_unlink routines.
 */
void _POSIX_Semaphore_Delete(
  POSIX_Semaphore_Control *the_semaphore,
  Thread_queue_Context    *queue_context
);

/**
 * @brief POSIX semaphore wait support.
 *
 * This routine supports the sem_wait, sem_trywait, and sem_timedwait
 * services.
 */
int _POSIX_Semaphore_Wait_support(
  sem_t               *sem,
  bool                 blocking,
  Watchdog_Interval    timeout
);
 
/**
 *  @brief POSIX Semaphore Namespace Remove
 */
RTEMS_INLINE_ROUTINE void _POSIX_Semaphore_Namespace_remove (
  POSIX_Semaphore_Control *the_semaphore
)
{
  _Objects_Namespace_remove( 
    &_POSIX_Semaphore_Information, &the_semaphore->Object );
}

RTEMS_INLINE_ROUTINE POSIX_Semaphore_Control *_POSIX_Semaphore_Get_by_name(
  const char                *name,
  size_t                    *name_length_p,
  Objects_Get_by_name_error *error
)
{
  return (POSIX_Semaphore_Control *) _Objects_Get_by_name(
    &_POSIX_Semaphore_Information,
    name,
    name_length_p,
    error
  );
}

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
