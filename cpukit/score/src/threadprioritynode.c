/*
 * Thread Priority Node
 * 
 * Copyright (C) 2017. Gedare Bloom.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/chain.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/thread.h>

/**
 * This routine adds a thread's Priority_node to the mutex holder's
 * Inherited_priorities list, and sets the waiting_to_hold field to the
 * mutex.
 *
 * Returns true if the enqueued node is at the head of the holder's
 * Inherited priorities, and therefore the holder's priority may change.
 */
bool _Thread_Enqueue_priority_node(
  Thread_Control     *the_thread,
  CORE_mutex_Control *the_mutex
)
{
  Chain_Node *iter;
  Thread_Priority_node *thread_pn, *holder_pn;
  Priority_Control task_priority, iter_priority;
  
  thread_pn = &the_thread->Priority_node;
  holder_pn = &the_mutex->holder->Priority_node;

  thread_pn->waiting_to_hold = the_mutex;
  iter = holder_pn->Inherited_priorities.first;
  task_priority = thread_pn->current_priority;

  /* Find the first node in holder's Inherited_priorities list with
   * lower priority than the task, or find the end of the list. */
  while ( !_Chain_Is_tail( &holder_pn->Inherited_priorities, iter) ) {
    iter_priority = ((Thread_Priority_node *)iter)->current_priority;
    if ( iter_priority > task_priority ) {
      break;
    }
    iter = _Chain_Next( iter );
  }
  _Chain_Insert_unprotected( _Chain_Previous( iter ), &thread_pn->Node );

  /* If the_thread is at the start of the holder's Inherited_priorites list,
   * then may need to update the holder's current priority. */
  if ( _Chain_Is_first( &thread_pn->Node ) ) {
    return true;
  }
  return false;
}

/**
 * This routine sets the thread's priority_node->current_priority to the
 * minimum of the real_priority and the first node of Inherited_priorities,
 * if any.
 */
void _Thread_Evaluate_priority(
  Thread_Control *the_thread
)
{
  Chain_Node *first;
  Thread_Priority_node *first_pn, *priority_node;
  Priority_Control current_priority, min_priority;

  /* Start with the_thread, and iterate through the holder threads that
   * the_thread is waiting on. If any thread's priority does not change,
   * then stop iterating. */
  while ( the_thread != NULL ) {
    priority_node = &the_thread->Priority_node;
    current_priority = priority_node->current_priority;
    min_priority = priority_node->real_priority;
    first = _Chain_First( &priority_node->Inherited_priorities );
    if ( !_Chain_Is_tail( &priority_node->Inherited_priorities, first ) ) {
      first_pn = (Thread_Priority_node *)first;
      if ( min_priority > first_pn->current_priority ) {
        min_priority = first_pn->current_priority;
      }
    }
    if ( current_priority < min_priority ) {
      _Thread_Change_priority( the_thread, min_priority, true );
    } else if ( current_priority > min_priority ) {
      _Thread_Change_priority( the_thread, min_priority, false );
    } else {
      break;
    }
    if ( the_thread->Priority_node.waiting_to_hold != NULL ) {
      the_thread = the_thread->Priority_node.waiting_to_hold->holder;
    } else {
      the_thread = NULL;
    }
  }
}

/**
 * This routine removes a thread's Priority_node from an
 * Inherited_priorities list, updates the mutex holder's
 * current_priority if needed, and clears the mutex field.
 *
 * Returns the mutex that the_thread was waiting on.
 */
CORE_mutex_Control* _Thread_Dequeue_priority_node(
  Thread_Priority_node *thread_pn
)
{
  CORE_mutex_Control *the_mutex = thread_pn->waiting_to_hold;

  if ( the_mutex == NULL ) {
    return NULL;
  }

  _Chain_Extract_unprotected( &thread_pn->Node );
  thread_pn->waiting_to_hold = NULL;

  return the_mutex;
}

void _Thread_Requeue_priority_node(
  Thread_Control *the_thread
)
{
  ISR_Level       level;
  CORE_mutex_Control *mutex;
  
  _ISR_Disable( level );
  /* TODO: refactor? */
  mutex = _Thread_Dequeue_priority_node( &the_thread->Priority_node );
  if ( mutex != NULL ) {
    _Thread_Enqueue_priority_node( the_thread, mutex );
  }
  _ISR_Enable( level );
}

/**
 * This routine removes the inherited priorities from the mutex being
 * released and updates the holder's priority if necessary.
 */
void _Thread_Release_inherited_priority(
  CORE_mutex_Control *the_mutex
)
{
  Chain_Control *ip = &the_mutex->holder->Priority_node.Inherited_priorities;
  Chain_Node *cn;
  Thread_Priority_node *tpn;

  for (cn = _Chain_First(ip); !_Chain_Is_tail(ip, cn); cn = _Chain_Next(cn)) {
    tpn = (Thread_Priority_node*)cn;
    if ( tpn->waiting_to_hold == the_mutex ) {
      _Chain_Extract(cn);
      tpn->waiting_to_hold = NULL;
    }
  }

  _Thread_Evaluate_priority( the_mutex->holder );
}

