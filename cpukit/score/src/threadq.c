/**
 * @file
 *
 * @ingroup RTEMSScoreThreadQueue
 *
 * @brief This source file contains static assertions related to the Thread
 *   Queue Handler, the definition of ::_Thread_queue_Object_name, and the
 *   implementation of _Thread_queue_Acquire(),
 *   _Thread_queue_Do_acquire_critical(), _Thread_queue_Initialize(),
 *   _Thread_queue_MP_callout_do_nothing(), _Thread_queue_Object_initialize(),
 *   _Thread_queue_Queue_get_name_and_id(), _Thread_queue_Release(), and
 *   _Thread_queue_Release_critical().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include <rtems/score/threadqimpl.h>
#include <rtems/score/rbtreeimpl.h>
#include <rtems/score/threadimpl.h>

RTEMS_STATIC_ASSERT(
#if defined(RTEMS_SMP)
  offsetof( Thread_queue_Syslock_queue, Queue.Lock.next_ticket )
#else
  offsetof( Thread_queue_Syslock_queue, reserved[ 0 ] )
#endif
    == offsetof( struct _Thread_queue_Queue, _Lock._next_ticket ),
  THREAD_QUEUE_SYSLOCK_QUEUE_NEXT_TICKET
);

RTEMS_STATIC_ASSERT(
#if defined(RTEMS_SMP)
  offsetof( Thread_queue_Syslock_queue, Queue.Lock.now_serving )
#else
  offsetof( Thread_queue_Syslock_queue, reserved[ 1 ] )
#endif
    == offsetof( struct _Thread_queue_Queue, _Lock._now_serving ),
  THREAD_QUEUE_SYSLOCK_QUEUE_NOW_SERVING
);

RTEMS_STATIC_ASSERT(
  offsetof( Thread_queue_Syslock_queue, Queue.heads )
    == offsetof( struct _Thread_queue_Queue, _heads ),
  THREAD_QUEUE_SYSLOCK_QUEUE_HEADS
);

RTEMS_STATIC_ASSERT(
  offsetof( Thread_queue_Syslock_queue, Queue.owner )
    == offsetof( struct _Thread_queue_Queue, _owner ),
  THREAD_QUEUE_SYSLOCK_QUEUE_OWNER
);

RTEMS_STATIC_ASSERT(
  offsetof( Thread_queue_Syslock_queue, Queue.name )
    == offsetof( struct _Thread_queue_Queue, _name ),
  THREAD_QUEUE_SYSLOCK_QUEUE_NAME
);

RTEMS_STATIC_ASSERT(
  sizeof( Thread_queue_Syslock_queue )
    == sizeof( struct _Thread_queue_Queue ),
  THREAD_QUEUE_SYSLOCK_QUEUE_SIZE
);

#if defined(RTEMS_SMP)
void _Thread_queue_Do_acquire_critical(
  Thread_queue_Control *the_thread_queue,
  ISR_lock_Context     *lock_context
)
{
  _Thread_queue_Queue_acquire_critical(
    &the_thread_queue->Queue,
    &the_thread_queue->Lock_stats,
    lock_context
  );
#if defined(RTEMS_DEBUG)
  the_thread_queue->owner = _SMP_lock_Who_am_I();
#endif
}

void _Thread_queue_Acquire(
  Thread_queue_Control *the_thread_queue,
  Thread_queue_Context *queue_context
)
{
  _ISR_lock_ISR_disable( &queue_context->Lock_context.Lock_context );
  _Thread_queue_Queue_acquire_critical(
    &the_thread_queue->Queue,
    &the_thread_queue->Lock_stats,
    &queue_context->Lock_context.Lock_context
  );
#if defined(RTEMS_DEBUG)
  the_thread_queue->owner = _SMP_lock_Who_am_I();
#endif
}

void _Thread_queue_Do_release_critical(
  Thread_queue_Control *the_thread_queue,
  ISR_lock_Context     *lock_context
)
{
#if defined(RTEMS_DEBUG)
  _Assert( _Thread_queue_Is_lock_owner( the_thread_queue ) );
  the_thread_queue->owner = SMP_LOCK_NO_OWNER;
#endif
  _Thread_queue_Queue_release_critical(
    &the_thread_queue->Queue,
    lock_context
  );
}

void _Thread_queue_Release(
  Thread_queue_Control *the_thread_queue,
  Thread_queue_Context *queue_context
)
{
#if defined(RTEMS_DEBUG)
  _Assert( _Thread_queue_Is_lock_owner( the_thread_queue ) );
  the_thread_queue->owner = SMP_LOCK_NO_OWNER;
#endif
  _Thread_queue_Queue_release_critical(
    &the_thread_queue->Queue,
    &queue_context->Lock_context.Lock_context
  );
  _ISR_lock_ISR_enable( &queue_context->Lock_context.Lock_context );
}
#endif

const char _Thread_queue_Object_name[] = { '\0' };

void _Thread_queue_Initialize(
  Thread_queue_Control *the_thread_queue,
  const char           *name
)
{
  _Thread_queue_Queue_initialize( &the_thread_queue->Queue, name );
#if defined(RTEMS_SMP)
  _SMP_lock_Stats_initialize( &the_thread_queue->Lock_stats, "Thread Queue" );
#endif
}

void _Thread_queue_Object_initialize( Thread_queue_Control *the_thread_queue )
{
  _Thread_queue_Initialize( the_thread_queue, _Thread_queue_Object_name );
}

#if defined(RTEMS_MULTIPROCESSING)
void _Thread_queue_MP_callout_do_nothing(
  Thread_Control *the_proxy,
  Objects_Id      mp_id
)
{
  /* Do nothing */
}
#endif

size_t _Thread_queue_Queue_get_name_and_id(
  const Thread_queue_Queue *queue,
  char                     *buffer,
  size_t                    buffer_size,
  Objects_Id               *id
)
{
  const char *name;

  name = queue->name;

  if ( name == _Thread_queue_Object_name ) {
    const Thread_queue_Object *queue_object;

    queue_object = THREAD_QUEUE_QUEUE_TO_OBJECT( queue );
    *id = queue_object->Object.id;
    return _Objects_Name_to_string(
      queue_object->Object.name,
      false,
      buffer,
      buffer_size
    );
  } else {
    if ( name == NULL ) {
      name = _Thread_queue_Object_name;
    }

    *id = 0;
    return strlcpy( buffer, name, buffer_size );
  }
}
