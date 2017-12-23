/**
 * @file
 *
 * @brief Private Inlined Routines for POSIX Mutex's.
 * 
 * This include file contains the static inline implementation of the private 
 * inlined routines for POSIX mutex's.
 */

/*  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_MUTEXIMPL_H
#define _RTEMS_POSIX_MUTEXIMPL_H

#include <errno.h>
#include <pthread.h>

#include <rtems/score/percpu.h>
#include <rtems/score/muteximpl.h>
#include <rtems/score/threadimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned long flags;
  Mutex_recursive_Control Recursive;
  Priority_Node Priority_ceiling;
  const Scheduler_Control *scheduler;
} POSIX_Mutex_Control;

#define POSIX_MUTEX_PROTOCOL_MASK 0x3UL

#define POSIX_MUTEX_RECURSIVE 0x4UL

#define POSIX_MUTEX_FLAGS_MASK 0x7UL

#define POSIX_MUTEX_MAGIC 0x961c13b8UL

#define POSIX_MUTEX_NO_PROTOCOL_TQ_OPERATIONS &_Thread_queue_Operations_FIFO

#define POSIX_MUTEX_PRIORITY_INHERIT_TQ_OPERATIONS \
  &_Thread_queue_Operations_priority_inherit

#define POSIX_MUTEX_PRIORITY_CEILING_TQ_OPERATIONS \
  &_Thread_queue_Operations_priority

/**
 * @brief Supported POSIX mutex protocols.
 *
 * Must be in synchronization with POSIX_Mutex_Control::protocol.
 */
typedef enum {
  POSIX_MUTEX_NO_PROTOCOL,
  POSIX_MUTEX_PRIORITY_INHERIT,
  POSIX_MUTEX_PRIORITY_CEILING
} POSIX_Mutex_Protocol;

/**
 *  The default mutex attributes structure.
 */
extern const pthread_mutexattr_t _POSIX_Mutex_Default_attributes;

RTEMS_INLINE_ROUTINE Thread_Control *_POSIX_Mutex_Acquire(
  POSIX_Mutex_Control  *the_mutex,
  Thread_queue_Context *queue_context
)
{
  ISR_Level       level;
  Thread_Control *executing;

  _Thread_queue_Context_initialize( queue_context );
  _Thread_queue_Context_ISR_disable( queue_context, level );
  _Thread_queue_Context_set_ISR_level( queue_context, level );
  executing = _Thread_Executing;
  _Thread_queue_Queue_acquire_critical(
    &the_mutex->Recursive.Mutex.Queue.Queue,
    &executing->Potpourri_stats,
    &queue_context->Lock_context.Lock_context
  );

  return executing;
}

RTEMS_INLINE_ROUTINE void _POSIX_Mutex_Release(
  POSIX_Mutex_Control  *the_mutex,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Queue_release(
    &the_mutex->Recursive.Mutex.Queue.Queue,
    &queue_context->Lock_context.Lock_context
  );
}

RTEMS_INLINE_ROUTINE POSIX_Mutex_Protocol _POSIX_Mutex_Get_protocol(
  unsigned long flags
)
{
  return flags & POSIX_MUTEX_PROTOCOL_MASK;
}

RTEMS_INLINE_ROUTINE bool _POSIX_Mutex_Is_recursive(
  unsigned long flags
)
{
  return ( flags & POSIX_MUTEX_RECURSIVE ) != 0;
}

RTEMS_INLINE_ROUTINE Thread_Control *_POSIX_Mutex_Get_owner(
  const POSIX_Mutex_Control *the_mutex
)
{
  return the_mutex->Recursive.Mutex.Queue.Queue.owner;
}

RTEMS_INLINE_ROUTINE bool _POSIX_Mutex_Is_locked(
  const POSIX_Mutex_Control *the_mutex
)
{
  return _POSIX_Mutex_Get_owner( the_mutex ) != NULL;
}

Status_Control _POSIX_Mutex_Seize_slow(
  POSIX_Mutex_Control           *the_mutex,
  const Thread_queue_Operations *operations,
  Thread_Control                *executing,
  const struct timespec         *abstime,
  Thread_queue_Context          *queue_context
);

RTEMS_INLINE_ROUTINE void _POSIX_Mutex_Set_owner(
  POSIX_Mutex_Control *the_mutex,
  Thread_Control      *owner
)
{
  the_mutex->Recursive.Mutex.Queue.Queue.owner = owner;
}

RTEMS_INLINE_ROUTINE bool _POSIX_Mutex_Is_owner(
  const POSIX_Mutex_Control *the_mutex,
  const Thread_Control      *the_thread
)
{
  return _POSIX_Mutex_Get_owner( the_mutex ) == the_thread;
}

static Status_Control _POSIX_Mutex_Lock_nested(
  POSIX_Mutex_Control *the_mutex,
  unsigned long        flags
)
{

  if ( _POSIX_Mutex_Is_recursive( flags ) ) {
    ++the_mutex->Recursive.nest_level;
    return STATUS_SUCCESSFUL;
  } else {
    return STATUS_NESTING_NOT_ALLOWED;
  }
}

RTEMS_INLINE_ROUTINE Status_Control _POSIX_Mutex_Seize(
  POSIX_Mutex_Control           *the_mutex,
  unsigned long                  flags,
  const Thread_queue_Operations *operations,
  Thread_Control                *executing,
  const struct timespec         *abstime,
  Thread_queue_Context          *queue_context
)
{
  Thread_Control *owner;

  owner = _POSIX_Mutex_Get_owner( the_mutex );

  if ( owner == NULL ) {
    _POSIX_Mutex_Set_owner( the_mutex, executing );
    _Thread_Resource_count_increment( executing );
    _POSIX_Mutex_Release( the_mutex, queue_context );
    return STATUS_SUCCESSFUL;
  }

  if ( owner == executing ) {
    Status_Control status;

    status = _POSIX_Mutex_Lock_nested( the_mutex, flags );
    _POSIX_Mutex_Release( the_mutex, queue_context );
    return status;
  }

  return _POSIX_Mutex_Seize_slow(
    the_mutex,
    operations,
    executing,
    abstime,
    queue_context
  );
}

RTEMS_INLINE_ROUTINE Status_Control _POSIX_Mutex_Surrender(
  POSIX_Mutex_Control           *the_mutex,
  const Thread_queue_Operations *operations,
  Thread_Control                *executing,
  Thread_queue_Context          *queue_context
)
{
  unsigned int        nest_level;
  Thread_queue_Heads *heads;

  if ( !_POSIX_Mutex_Is_owner( the_mutex, executing ) ) {
    _POSIX_Mutex_Release( the_mutex, queue_context );
    return STATUS_NOT_OWNER;
  }

  nest_level = the_mutex->Recursive.nest_level;

  if ( nest_level > 0 ) {
    the_mutex->Recursive.nest_level = nest_level - 1;
    _POSIX_Mutex_Release( the_mutex, queue_context );
    return STATUS_SUCCESSFUL;
  }

  _Thread_Resource_count_decrement( executing );
  _POSIX_Mutex_Set_owner( the_mutex, NULL );

  heads = the_mutex->Recursive.Mutex.Queue.Queue.heads;

  if ( heads == NULL ) {
    _POSIX_Mutex_Release( the_mutex, queue_context );
    return STATUS_SUCCESSFUL;
  }

  _Thread_queue_Surrender(
    &the_mutex->Recursive.Mutex.Queue.Queue,
    heads,
    executing,
    queue_context,
    operations
  );
  return STATUS_SUCCESSFUL;
}

RTEMS_INLINE_ROUTINE const Scheduler_Control *_POSIX_Mutex_Get_scheduler(
  const POSIX_Mutex_Control *the_mutex
)
{
#if defined(RTEMS_SMP)
  return the_mutex->scheduler;
#else
  return &_Scheduler_Table[ 0 ];
#endif
}

RTEMS_INLINE_ROUTINE void _POSIX_Mutex_Set_priority(
  POSIX_Mutex_Control  *the_mutex,
  Priority_Control      priority_ceiling,
  Thread_queue_Context *queue_context
)
{
  Thread_Control *owner;

  owner = _POSIX_Mutex_Get_owner( the_mutex );

  if ( owner != NULL ) {
    _Thread_Wait_acquire( owner, queue_context );
    _Thread_Priority_change(
      owner,
      &the_mutex->Priority_ceiling,
      priority_ceiling,
      false,
      queue_context
    );
    _Thread_Wait_release( owner, queue_context );
  } else {
    the_mutex->Priority_ceiling.priority = priority_ceiling;
  }
}

RTEMS_INLINE_ROUTINE Priority_Control _POSIX_Mutex_Get_priority(
  const POSIX_Mutex_Control *the_mutex
)
{
  return the_mutex->Priority_ceiling.priority;
}

RTEMS_INLINE_ROUTINE Status_Control _POSIX_Mutex_Ceiling_set_owner(
  POSIX_Mutex_Control  *the_mutex,
  Thread_Control       *owner,
  Thread_queue_Context *queue_context
)
{
  ISR_lock_Context  lock_context;
  Scheduler_Node   *scheduler_node;
  Per_CPU_Control  *cpu_self;

  _Thread_Wait_acquire_default_critical( owner, &lock_context );

  scheduler_node = _Thread_Scheduler_get_home_node( owner );

  if (
    _Priority_Get_priority( &scheduler_node->Wait.Priority )
      < the_mutex->Priority_ceiling.priority
  ) {
    _Thread_Wait_release_default_critical( owner, &lock_context );
    _POSIX_Mutex_Release( the_mutex, queue_context );
    return STATUS_MUTEX_CEILING_VIOLATED;
  }

  _POSIX_Mutex_Set_owner( the_mutex, owner );
  _Thread_Resource_count_increment( owner );
  _Thread_Priority_add(
    owner,
    &the_mutex->Priority_ceiling,
    queue_context
  );
  _Thread_Wait_release_default_critical( owner, &lock_context );

  cpu_self = _Thread_queue_Dispatch_disable( queue_context );
  _POSIX_Mutex_Release( the_mutex, queue_context );
  _Thread_Priority_update( queue_context );
  _Thread_Dispatch_enable( cpu_self );
  return STATUS_SUCCESSFUL;
}

RTEMS_INLINE_ROUTINE Status_Control _POSIX_Mutex_Ceiling_seize(
  POSIX_Mutex_Control   *the_mutex,
  unsigned long          flags,
  Thread_Control        *executing,
  const struct timespec *abstime,
  Thread_queue_Context  *queue_context
)
{
  Thread_Control *owner;

  owner = _POSIX_Mutex_Get_owner( the_mutex );

  if ( owner == NULL ) {
#if defined(RTEMS_SMP)
    if (
      _Thread_Scheduler_get_home( executing )
        != _POSIX_Mutex_Get_scheduler( the_mutex )
    ) {
      _POSIX_Mutex_Release( the_mutex, queue_context );
      return STATUS_NOT_DEFINED;
    }
#endif

    _Thread_queue_Context_clear_priority_updates( queue_context );
    return _POSIX_Mutex_Ceiling_set_owner(
      the_mutex,
      executing,
      queue_context
    );
  }

  if ( owner == executing ) {
    Status_Control status;

    status = _POSIX_Mutex_Lock_nested( the_mutex, flags );
    _POSIX_Mutex_Release( the_mutex, queue_context );
    return status;
  }

  return _POSIX_Mutex_Seize_slow(
    the_mutex,
    POSIX_MUTEX_PRIORITY_CEILING_TQ_OPERATIONS,
    executing,
    abstime,
    queue_context
  );
}

RTEMS_INLINE_ROUTINE Status_Control _POSIX_Mutex_Ceiling_surrender(
  POSIX_Mutex_Control  *the_mutex,
  Thread_Control       *executing,
  Thread_queue_Context *queue_context
)
{
  unsigned int        nest_level;
  ISR_lock_Context    lock_context;
  Per_CPU_Control    *cpu_self;
  Thread_queue_Heads *heads;

  if ( !_POSIX_Mutex_Is_owner( the_mutex, executing ) ) {
    _POSIX_Mutex_Release( the_mutex, queue_context );
    return STATUS_NOT_OWNER;
  }

  nest_level = the_mutex->Recursive.nest_level;

  if ( nest_level > 0 ) {
    the_mutex->Recursive.nest_level = nest_level - 1;
    _POSIX_Mutex_Release( the_mutex, queue_context );
    return STATUS_SUCCESSFUL;
  }

  _Thread_Resource_count_decrement( executing );

  _Thread_queue_Context_clear_priority_updates( queue_context );
  _Thread_Wait_acquire_default_critical( executing, &lock_context );
  _Thread_Priority_remove(
    executing,
    &the_mutex->Priority_ceiling,
    queue_context
  );
  _Thread_Wait_release_default_critical( executing, &lock_context );

  cpu_self = _Thread_queue_Dispatch_disable( queue_context );

  heads = the_mutex->Recursive.Mutex.Queue.Queue.heads;

  if ( heads != NULL ) {
    const Thread_queue_Operations *operations;
    Thread_Control                *new_owner;

    operations = POSIX_MUTEX_PRIORITY_CEILING_TQ_OPERATIONS;
    new_owner = ( *operations->first )( heads );
    _POSIX_Mutex_Set_owner( the_mutex, new_owner );
    _Thread_Resource_count_increment( new_owner );
    _Thread_Priority_add(
      new_owner,
      &the_mutex->Priority_ceiling,
      queue_context
    );
    _Thread_queue_Extract_critical(
      &the_mutex->Recursive.Mutex.Queue.Queue,
      operations,
      new_owner,
      queue_context
    );
  } else {
    _POSIX_Mutex_Set_owner( the_mutex, NULL );
    _POSIX_Mutex_Release( the_mutex, queue_context );
  }

  _Thread_Priority_update( queue_context );
  _Thread_Dispatch_enable( cpu_self );
  return STATUS_SUCCESSFUL;
}

#define POSIX_MUTEX_ABSTIME_TRY_LOCK ((uintptr_t) 1)

int _POSIX_Mutex_Lock_support(
  pthread_mutex_t              *mutex,
  const struct timespec        *abstime,
  Thread_queue_Enqueue_callout  enqueue_callout
);

static inline POSIX_Mutex_Control *_POSIX_Mutex_Get(
  pthread_mutex_t *mutex
)
{
  return (POSIX_Mutex_Control *) mutex;
}

bool _POSIX_Mutex_Auto_initialization( POSIX_Mutex_Control *the_mutex );

#define POSIX_MUTEX_VALIDATE_OBJECT( the_mutex, flags ) \
  do { \
    if ( ( the_mutex ) == NULL ) { \
      return EINVAL; \
    } \
    flags = ( the_mutex )->flags; \
    if ( \
      ( ( (uintptr_t) ( the_mutex ) ^ POSIX_MUTEX_MAGIC ) \
          & ~POSIX_MUTEX_FLAGS_MASK ) \
        != ( flags & ~POSIX_MUTEX_FLAGS_MASK ) \
    ) { \
      if ( !_POSIX_Mutex_Auto_initialization( the_mutex ) ) { \
        return EINVAL; \
      } \
    } \
  } while ( 0 )

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */

