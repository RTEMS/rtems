/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThreadQueue
 *
 * @brief This source file contains the implementation of
 *   _Thread_queue_Deadlock_fatal(), _Thread_queue_Deadlock_status(),
 *   _Thread_queue_Do_dequeue(), _Thread_queue_Enqueue(),
 *   _Thread_queue_Enqueue_do_nothing_extra(), _Thread_queue_Enqueue_sticky(),
 *   _Thread_queue_Extract_locked(), _Thread_queue_Path_acquire(),
 *   _Thread_queue_Path_release(),
 *   _Thread_queue_Resume(),_Thread_queue_Surrender(),
 *   _Thread_queue_Surrender_no_priority(), _Thread_queue_Surrender_sticky().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (C) 2015, 2016 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadqimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/status.h>
#include <rtems/score/watchdogimpl.h>

#define THREAD_QUEUE_INTEND_TO_BLOCK \
  (THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_INTEND_TO_BLOCK)

#define THREAD_QUEUE_BLOCKED \
  (THREAD_WAIT_CLASS_OBJECT | THREAD_WAIT_STATE_BLOCKED)

#if defined(RTEMS_SMP)
/*
 * A global registry of active thread queue links is used to provide deadlock
 * detection on SMP configurations.  This is simple to implement and no
 * additional storage is required for the thread queues.  The disadvantage is
 * the global registry is not scalable and may lead to lock contention.
 * However, the registry is only used in case of nested resource conflicts.  In
 * this case, the application is already in trouble.
 */

typedef struct {
  ISR_lock_Control Lock;

  RBTree_Control Links;
} Thread_queue_Links;

static Thread_queue_Links _Thread_queue_Links = {
  ISR_LOCK_INITIALIZER( "Thread Queue Links" ),
  RBTREE_INITIALIZER_EMPTY( _Thread_queue_Links.Links )
};

static bool _Thread_queue_Link_equal(
  const void        *left,
  const RBTree_Node *right
)
{
  const Thread_queue_Queue *the_left;
  const Thread_queue_Link  *the_right;

  the_left = left;
  the_right = (Thread_queue_Link *) right;

  return the_left == the_right->source;
}

static bool _Thread_queue_Link_less(
  const void        *left,
  const RBTree_Node *right
)
{
  const Thread_queue_Queue *the_left;
  const Thread_queue_Link  *the_right;

  the_left = left;
  the_right = (Thread_queue_Link *) right;

  return (uintptr_t) the_left < (uintptr_t) the_right->source;
}

static void *_Thread_queue_Link_map( RBTree_Node *node )
{
  return node;
}

static Thread_queue_Link *_Thread_queue_Link_find(
  Thread_queue_Links *links,
  Thread_queue_Queue *source
)
{
  return _RBTree_Find_inline(
    &links->Links,
    source,
    _Thread_queue_Link_equal,
    _Thread_queue_Link_less,
    _Thread_queue_Link_map
  );
}

static Thread_queue_Deadlock_status _Thread_queue_Link_add(
  Thread_queue_Link  *link,
  Thread_queue_Queue *source,
  Thread_queue_Queue *target
)
{
  Thread_queue_Links *links;
  Thread_queue_Queue *recursive_target;
  ISR_lock_Context    lock_context;

  link->source = source;
  link->target = target;

  links = &_Thread_queue_Links;
  recursive_target = target;

  _ISR_lock_Acquire( &links->Lock, &lock_context );

  while ( true ) {
    Thread_queue_Link *recursive_link;

    recursive_link = _Thread_queue_Link_find( links, recursive_target );

    if ( recursive_link == NULL ) {
      break;
    }

    recursive_target = recursive_link->target;

    if ( recursive_target == source ) {
      _ISR_lock_Release( &links->Lock, &lock_context );
      return THREAD_QUEUE_DEADLOCK_DETECTED;
    }
  }

  _RBTree_Insert_inline(
    &links->Links,
    &link->Registry_node,
    source,
    _Thread_queue_Link_less
  );

  _ISR_lock_Release( &links->Lock, &lock_context );
  return THREAD_QUEUE_NO_DEADLOCK;
}

static void _Thread_queue_Link_remove( Thread_queue_Link *link )
{
  Thread_queue_Links *links;
  ISR_lock_Context    lock_context;

  links = &_Thread_queue_Links;

  _ISR_lock_Acquire( &links->Lock, &lock_context );
  _RBTree_Extract( &links->Links, &link->Registry_node );
  _ISR_lock_Release( &links->Lock, &lock_context );
}
#endif

#if !defined(RTEMS_SMP)
static
#endif
void _Thread_queue_Path_release( Thread_queue_Context *queue_context )
{
#if defined(RTEMS_SMP)
  Chain_Node *head;
  Chain_Node *node;

  head = _Chain_Head( &queue_context->Path.Links );
  node = _Chain_Last( &queue_context->Path.Links );

  while ( head != node ) {
    Thread_queue_Link *link;

    link = THREAD_QUEUE_LINK_OF_PATH_NODE( node );

    if ( link->Lock_context.Wait.queue != NULL ) {
      _Thread_queue_Link_remove( link );
      _Thread_Wait_release_queue_critical(
        link->Lock_context.Wait.queue,
        &link->Lock_context
      );
      _Thread_Wait_remove_request( link->owner, &link->Lock_context );
    } else {
      _Thread_Wait_release_default_critical(
        link->owner,
        &link->Lock_context.Lock_context
      );
    }

    node = _Chain_Previous( node );
#if defined(RTEMS_DEBUG)
    _Chain_Set_off_chain( &link->Path_node );
#endif
  }
#else
  (void) queue_context;
#endif
}

#if defined(RTEMS_SMP)
static void _Thread_queue_Path_append_deadlock_thread(
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  Thread_Control *deadlock;

  /*
   * In case of a deadlock, we must obtain the thread wait default lock for the
   * first thread on the path that tries to enqueue on a thread queue.  This
   * thread can be identified by the thread wait operations.  This lock acquire
   * is necessary for the timeout and explicit thread priority changes, see
   * _Thread_Priority_perform_actions().
   */

  deadlock = NULL;

  while ( the_thread->Wait.operations != &_Thread_queue_Operations_default ) {
    the_thread = the_thread->Wait.queue->owner;
    deadlock = the_thread;
  }

  if ( deadlock != NULL ) {
    Thread_queue_Link *link;

    link = &queue_context->Path.Deadlock;
    _Chain_Initialize_node( &link->Path_node );
    _Chain_Append_unprotected(
      &queue_context->Path.Links,
      &link->Path_node
    );
    link->owner = deadlock;
    link->Lock_context.Wait.queue = NULL;
    _Thread_Wait_acquire_default_critical(
      deadlock,
      &link->Lock_context.Lock_context
    );
  }
}
#endif

#if !defined(RTEMS_SMP)
static
#endif
Thread_queue_Deadlock_status _Thread_queue_Path_acquire(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  Thread_Control     *owner;
#if defined(RTEMS_SMP)
  Thread_queue_Link  *link;
  Thread_queue_Queue *target;

  /*
   * For an overview please look at the non-SMP part below.  In SMP
   * configurations, we basically do the same.  The fact that we may have more
   * than one executing thread and each thread queue has its own SMP lock makes
   * the procedure a bit more difficult.  We have to avoid deadlocks at SMP
   * lock level, since this would result in an unrecoverable deadlock of the
   * overall system.
   */

  _Chain_Initialize_empty( &queue_context->Path.Links );

  owner = queue->owner;

  if ( owner == NULL ) {
    return THREAD_QUEUE_NO_DEADLOCK;
  }

  if ( owner == the_thread ) {
    return THREAD_QUEUE_DEADLOCK_DETECTED;
  }

  _Chain_Initialize_node(
    &queue_context->Path.Start.Lock_context.Wait.Gate.Node
  );
  link = &queue_context->Path.Start;
  _RBTree_Initialize_node( &link->Registry_node );
  _Chain_Initialize_node( &link->Path_node );

  do {
    _Chain_Append_unprotected( &queue_context->Path.Links, &link->Path_node );
    link->owner = owner;

    _Thread_Wait_acquire_default_critical(
      owner,
      &link->Lock_context.Lock_context
    );

    target = owner->Wait.queue;
    link->Lock_context.Wait.queue = target;

    if ( target != NULL ) {
      Thread_queue_Deadlock_status deadlock_status;

      deadlock_status = _Thread_queue_Link_add( link, queue, target );

      if ( deadlock_status == THREAD_QUEUE_NO_DEADLOCK ) {
        _Thread_queue_Gate_add(
          &owner->Wait.Lock.Pending_requests,
          &link->Lock_context.Wait.Gate
        );
        _Thread_Wait_release_default_critical(
          owner,
          &link->Lock_context.Lock_context
        );
        _Thread_Wait_acquire_queue_critical( target, &link->Lock_context );

        if ( link->Lock_context.Wait.queue == NULL ) {
          _Thread_queue_Link_remove( link );
          _Thread_Wait_release_queue_critical( target, &link->Lock_context );
          _Thread_Wait_acquire_default_critical(
            owner,
            &link->Lock_context.Lock_context
          );
          _Thread_Wait_remove_request_locked( owner, &link->Lock_context );
          _Assert( owner->Wait.queue == NULL );
          return THREAD_QUEUE_NO_DEADLOCK;
        }
      } else {
        link->Lock_context.Wait.queue = NULL;
        _Thread_queue_Path_append_deadlock_thread( owner, queue_context );
        return THREAD_QUEUE_DEADLOCK_DETECTED;
      }
    } else {
      return THREAD_QUEUE_NO_DEADLOCK;
    }

    link = &owner->Wait.Link;
    queue = target;
    owner = queue->owner;
  } while ( owner != NULL );
#else
  do {
    owner = queue->owner;

    if ( owner == NULL ) {
      return THREAD_QUEUE_NO_DEADLOCK;
    }

    if ( owner == the_thread ) {
      return THREAD_QUEUE_DEADLOCK_DETECTED;
    }

    queue = owner->Wait.queue;
  } while ( queue != NULL );
#endif

  return THREAD_QUEUE_NO_DEADLOCK;
}

void _Thread_queue_Enqueue_do_nothing_extra(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Per_CPU_Control      *cpu_self,
  Thread_queue_Context *queue_context
)
{
  /* Do nothing */
}

void _Thread_queue_Deadlock_status( Thread_Control *the_thread )
{
  the_thread->Wait.return_code = STATUS_DEADLOCK;
}

void _Thread_queue_Deadlock_fatal( Thread_Control *the_thread )
{
  _Internal_error( INTERNAL_ERROR_THREAD_QUEUE_DEADLOCK );
}

void _Thread_queue_Enqueue(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_Control                *the_thread,
  Thread_queue_Context          *queue_context
)
{
  Thread_queue_Deadlock_status deadlock_status;
  Per_CPU_Control             *cpu_self;
  bool                         success;

  _Assert( queue_context->enqueue_callout != NULL );

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Thread_MP_Is_receive( the_thread ) && the_thread->receive_packet ) {
    the_thread = _Thread_MP_Allocate_proxy( queue_context->thread_state );
  }
#endif

  _Thread_Wait_claim( the_thread, queue );

  deadlock_status =
    _Thread_queue_Path_acquire( queue, the_thread, queue_context );

  if ( deadlock_status == THREAD_QUEUE_DEADLOCK_DETECTED ) {
    _Thread_queue_Path_release( queue_context );
    _Thread_Wait_restore_default( the_thread );
    _Thread_queue_Queue_release( queue, &queue_context->Lock_context.Lock_context );
    _Thread_Wait_tranquilize( the_thread );
    _Assert( queue_context->deadlock_callout != NULL );
    ( *queue_context->deadlock_callout )( the_thread );
    return;
  }

  _Thread_queue_Context_clear_priority_updates( queue_context );
  _Thread_Wait_claim_finalize( the_thread, operations );
  ( *operations->enqueue )( queue, the_thread, queue_context );

  _Thread_queue_Path_release( queue_context );

  the_thread->Wait.return_code = STATUS_SUCCESSFUL;
  _Thread_Wait_flags_set( the_thread, THREAD_QUEUE_INTEND_TO_BLOCK );
  cpu_self = _Thread_queue_Dispatch_disable( queue_context );
  _Thread_queue_Queue_release( queue, &queue_context->Lock_context.Lock_context );

  ( *queue_context->enqueue_callout )(
    queue,
    the_thread,
    cpu_self,
    queue_context
  );

  /*
   *  Set the blocking state for this thread queue in the thread.
   */
  _Thread_Set_state( the_thread, queue_context->thread_state );

  /*
   * At this point thread dispatching is disabled, however, we already released
   * the thread queue lock.  Thus, interrupts or threads on other processors
   * may already changed our state with respect to the thread queue object.
   * The request could be satisfied or timed out.  This situation is indicated
   * by the thread wait flags.  Other parties must not modify our thread state
   * as long as we are in the THREAD_QUEUE_INTEND_TO_BLOCK thread wait state,
   * thus we have to cancel the blocking operation ourself if necessary.
   */
  success = _Thread_Wait_flags_try_change_acquire(
    the_thread,
    THREAD_QUEUE_INTEND_TO_BLOCK,
    THREAD_QUEUE_BLOCKED
  );
  if ( !success ) {
    _Thread_Remove_timer_and_unblock( the_thread, queue );
  }

  _Thread_Priority_update( queue_context );
  _Thread_Dispatch_direct( cpu_self );
}

#if defined(RTEMS_SMP)
Status_Control _Thread_queue_Enqueue_sticky(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_Control                *the_thread,
  Thread_queue_Context          *queue_context
)
{
  Thread_queue_Deadlock_status deadlock_status;
  Per_CPU_Control             *cpu_self;

  _Assert( queue_context->enqueue_callout != NULL );

  _Thread_Wait_claim( the_thread, queue );

  deadlock_status =
    _Thread_queue_Path_acquire( queue, the_thread, queue_context );

  if ( deadlock_status == THREAD_QUEUE_DEADLOCK_DETECTED ) {
    _Thread_queue_Path_release( queue_context );
    _Thread_Wait_restore_default( the_thread );
    _Thread_queue_Queue_release( queue, &queue_context->Lock_context.Lock_context );
    _Thread_Wait_tranquilize( the_thread );
    ( *queue_context->deadlock_callout )( the_thread );
    return _Thread_Wait_get_status( the_thread );
  }

  _Thread_queue_Context_clear_priority_updates( queue_context );
  _Thread_Wait_claim_finalize( the_thread, operations );
  ( *operations->enqueue )( queue, the_thread, queue_context );

  _Thread_queue_Path_release( queue_context );

  the_thread->Wait.return_code = STATUS_SUCCESSFUL;
  _Thread_Wait_flags_set( the_thread, THREAD_QUEUE_INTEND_TO_BLOCK );
  cpu_self = _Thread_queue_Dispatch_disable( queue_context );
  _Thread_queue_Queue_release( queue, &queue_context->Lock_context.Lock_context );

  if ( cpu_self->thread_dispatch_disable_level != 1 ) {
    _Internal_error(
      INTERNAL_ERROR_THREAD_QUEUE_ENQUEUE_STICKY_FROM_BAD_STATE
    );
  }

  ( *queue_context->enqueue_callout )(
    queue,
    the_thread,
    cpu_self,
    queue_context
  );

  _Thread_Priority_update( queue_context );
  _Thread_Priority_update_and_make_sticky( the_thread );
  _Thread_Dispatch_enable( cpu_self );

  while (
    _Thread_Wait_flags_get_acquire( the_thread ) == THREAD_QUEUE_INTEND_TO_BLOCK
  ) {
    /* Wait */
  }

  _Thread_Wait_tranquilize( the_thread );
  _Thread_Timer_remove( the_thread );
  return _Thread_Wait_get_status( the_thread );
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
bool _Thread_queue_MP_set_callout(
  Thread_Control             *the_thread,
  const Thread_queue_Context *queue_context
)
{
  Thread_Proxy_control    *the_proxy;
  Thread_queue_MP_callout  mp_callout;

  if ( _Objects_Is_local_id( the_thread->Object.id ) ) {
    return false;
  }

  the_proxy = (Thread_Proxy_control *) the_thread;
  mp_callout = queue_context->mp_callout;
  _Assert( mp_callout != NULL );
  the_proxy->thread_queue_callout = mp_callout;
  return true;
}
#endif

static void _Thread_queue_Force_ready_again( Thread_Control *the_thread )
{
  /*
   * We must set the wait flags under protection of the current thread lock,
   * otherwise a _Thread_Timeout() running on another processor may interfere.
   */
  _Thread_Wait_flags_set( the_thread, THREAD_WAIT_STATE_READY );
  _Thread_Wait_restore_default( the_thread );
}

static bool _Thread_queue_Make_ready_again( Thread_Control *the_thread )
{
  bool success;
  bool unblock;

  /*
   * We must update the wait flags under protection of the current thread lock,
   * otherwise a _Thread_Timeout() running on another processor may interfere.
   */
  success = _Thread_Wait_flags_try_change_release(
    the_thread,
    THREAD_QUEUE_INTEND_TO_BLOCK,
    THREAD_WAIT_STATE_READY
  );
  if ( success ) {
    unblock = false;
  } else {
    _Assert( _Thread_Wait_flags_get( the_thread ) == THREAD_QUEUE_BLOCKED );
    _Thread_Wait_flags_set( the_thread, THREAD_WAIT_STATE_READY );
    unblock = true;
  }

  _Thread_Wait_restore_default( the_thread );
  return unblock;
}

/*
 * This function is used instead of _Thread_queue_Make_ready_again() in
 * _Thread_queue_Surrender() and _Thread_queue_Surrender_priority_ceiling()
 * since only the previous owner thread is allowed to surrender the thread
 * queue.
 *
 * In uniprocessor configurations, there is only one executing thread (in this
 * case the previous owner), so the new owner thread must be fully blocked.
 *
 * In SMP configurations, the new owner may execute on another processor in
 * parallel, so we have to use _Thread_queue_Make_ready_again().
 */
static bool _Thread_queue_Make_new_owner_ready_again( Thread_Control *new_owner )
{
#if defined(RTEMS_SMP)
  return _Thread_queue_Make_ready_again( new_owner );
#else
  _Assert( _Thread_Wait_flags_get( new_owner ) == THREAD_QUEUE_BLOCKED );
  _Thread_queue_Force_ready_again( new_owner );
  return false;
#endif
}

static void _Thread_queue_Unblock_new_owner_and_remove_timer(
  Thread_queue_Queue *queue,
  Thread_Control     *new_owner,
  bool                unblock
)
{
#if defined(RTEMS_SMP)
  if ( unblock ) {
    _Thread_Remove_timer_and_unblock( new_owner, queue );
  }
#else
  (void) unblock;
  _Thread_Remove_timer_and_unblock( new_owner, queue );
#endif
}

bool _Thread_queue_Extract_locked(
  Thread_queue_Queue            *queue,
  const Thread_queue_Operations *operations,
  Thread_Control                *the_thread,
  Thread_queue_Context          *queue_context
)
{
#if defined(RTEMS_MULTIPROCESSING)
  _Thread_queue_MP_set_callout( the_thread, queue_context );
#endif
  ( *operations->extract )( queue, the_thread, queue_context );
  return _Thread_queue_Make_ready_again( the_thread );
}

void _Thread_queue_Resume(
  Thread_queue_Queue   *queue,
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  bool unblock;

  unblock = _Thread_queue_Make_ready_again( the_thread );

  if ( unblock ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_queue_Dispatch_disable( queue_context );
    _Thread_queue_Queue_release(
      queue, &queue_context->Lock_context.Lock_context
    );

    _Thread_Remove_timer_and_unblock( the_thread, queue );

    _Thread_Dispatch_enable( cpu_self );
  } else {
    _Thread_queue_Queue_release(
      queue, &queue_context->Lock_context.Lock_context
    );
  }
}

void _Thread_queue_Surrender(
  Thread_queue_Queue            *queue,
  Thread_queue_Heads            *heads,
  Thread_Control                *previous_owner,
  Thread_queue_Context          *queue_context,
  const Thread_queue_Operations *operations
)
{
  Thread_Control  *new_owner;
  bool             unblock;
  Per_CPU_Control *cpu_self;

  _Assert( heads != NULL );

  _Thread_queue_Context_clear_priority_updates( queue_context );
  new_owner = ( *operations->surrender )(
    queue,
    heads,
    previous_owner,
    queue_context
  );
  queue->owner = new_owner;

#if defined(RTEMS_MULTIPROCESSING)
  if ( !_Thread_queue_MP_set_callout( new_owner, queue_context ) )
#endif
  {
    _Thread_Resource_count_increment( new_owner );
  }

  unblock = _Thread_queue_Make_new_owner_ready_again( new_owner );

  cpu_self = _Thread_queue_Dispatch_disable( queue_context );
  _Thread_queue_Queue_release(
    queue,
    &queue_context->Lock_context.Lock_context
  );

  _Thread_Priority_update( queue_context );
  _Thread_queue_Unblock_new_owner_and_remove_timer(
    queue,
    new_owner,
    unblock
  );

  _Thread_Dispatch_enable( cpu_self );
}

void _Thread_queue_Surrender_no_priority(
  Thread_queue_Queue            *queue,
  Thread_queue_Heads            *heads,
  Thread_queue_Context          *queue_context,
  const Thread_queue_Operations *operations
)
{
  Thread_Control  *the_thread;

  _Assert( heads != NULL );
  _Assert( queue->owner == NULL );

  the_thread = ( *operations->surrender )( queue, heads, NULL, queue_context );

#if defined(RTEMS_MULTIPROCESSING)
  _Thread_queue_MP_set_callout( the_thread, queue_context );
#endif

  _Thread_queue_Resume( queue, the_thread, queue_context );
}

Status_Control _Thread_queue_Surrender_priority_ceiling(
  Thread_queue_Queue            *queue,
  Thread_Control                *executing,
  Priority_Node                 *priority_ceiling,
  Thread_queue_Context          *queue_context,
  const Thread_queue_Operations *operations
)
{
  ISR_lock_Context    lock_context;
  Thread_queue_Heads *heads;
  Thread_Control     *new_owner;
  bool                unblock;
  Per_CPU_Control    *cpu_self;

  _Thread_Resource_count_decrement( executing );

  _Thread_queue_Context_clear_priority_updates( queue_context );
  _Thread_Wait_acquire_default_critical( executing, &lock_context );
  _Thread_Priority_remove( executing, priority_ceiling, queue_context );
  _Thread_Wait_release_default_critical( executing, &lock_context );

  heads = queue->heads;
  queue->owner = NULL;

  if ( heads == NULL ) {
    cpu_self = _Thread_Dispatch_disable_critical(
      &queue_context->Lock_context.Lock_context
    );
    _Thread_queue_Queue_release(
      queue,
      &queue_context->Lock_context.Lock_context
    );
    _Thread_Priority_update( queue_context );
    _Thread_Dispatch_enable( cpu_self );
    return STATUS_SUCCESSFUL;
  }

  new_owner = ( *operations->surrender )(
    queue,
    heads,
    NULL,
    queue_context
  );

  queue->owner = new_owner;

  unblock = _Thread_queue_Make_new_owner_ready_again( new_owner );

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Objects_Is_local_id( new_owner->Object.id ) )
#endif
  {
    _Thread_Resource_count_increment( new_owner );
    _Thread_Wait_acquire_default_critical( new_owner, &lock_context );
    _Thread_Priority_add( new_owner, priority_ceiling, queue_context );
    _Thread_Wait_release_default_critical( new_owner, &lock_context );
  }

  cpu_self = _Thread_queue_Dispatch_disable( queue_context );
  _Thread_queue_Queue_release(
    queue,
    &queue_context->Lock_context.Lock_context
  );

  _Thread_Priority_update( queue_context );
  _Thread_queue_Unblock_new_owner_and_remove_timer(
    queue,
    new_owner,
    unblock
  );

  _Thread_Dispatch_enable( cpu_self );
  return STATUS_SUCCESSFUL;
}

#if defined(RTEMS_SMP)
void _Thread_queue_Surrender_sticky(
  Thread_queue_Queue            *queue,
  Thread_queue_Heads            *heads,
  Thread_Control                *previous_owner,
  Thread_queue_Context          *queue_context,
  const Thread_queue_Operations *operations
)
{
  Thread_Control  *new_owner;
  Per_CPU_Control *cpu_self;

  _Assert( heads != NULL );

  _Thread_queue_Context_clear_priority_updates( queue_context );
  new_owner = ( *operations->surrender )(
    queue,
    heads,
    previous_owner,
    queue_context
  );
  queue->owner = new_owner;

  /*
   * There is no need to unblock the thread, since in the corresponding
   * _Thread_queue_Enqueue_sticky() the thread is not blocked by the scheduler.
   * Instead, the thread busy waits for a change of its thread wait flags.
   * Timeouts cannot interfere since we hold the thread queue lock.
   */
  _Assert(
    _Thread_Wait_flags_get( new_owner ) == THREAD_QUEUE_INTEND_TO_BLOCK
  );
  _Thread_queue_Force_ready_again( new_owner );

  cpu_self = _Thread_queue_Dispatch_disable( queue_context );
  _Thread_queue_Queue_release(
    queue,
    &queue_context->Lock_context.Lock_context
  );
  _Thread_Priority_update_and_clean_sticky( previous_owner );
  _Thread_Priority_update_ignore_sticky( new_owner );
  _Thread_Dispatch_enable( cpu_self );
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
void _Thread_queue_Unblock_proxy(
  Thread_queue_Queue *queue,
  Thread_Control     *the_thread
)
{
  const Thread_queue_Object *the_queue_object;
  Thread_Proxy_control      *the_proxy;
  Thread_queue_MP_callout    mp_callout;

  the_queue_object = THREAD_QUEUE_QUEUE_TO_OBJECT( queue );
  the_proxy = (Thread_Proxy_control *) the_thread;
  mp_callout = the_proxy->thread_queue_callout;
  ( *mp_callout )( the_thread, the_queue_object->Object.id );

  _Thread_MP_Free_proxy( the_thread );
}
#endif
