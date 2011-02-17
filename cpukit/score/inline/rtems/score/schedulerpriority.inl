/** 
 *  @file  rtems/score/schedulerpriority.inl
 *
 *  This inline file contains all of the inlined routines associated with
 *  the manipulation of the priority-based scheduling structures.
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_SCHEDULERPRIORITY_H
# error "Never use <rtems/score/schedulerpriority.inl> directly; include <rtems/score/schedulerpriority.h> instead."
#endif

#ifndef _RTEMS_SCORE_SCHEDULERPRIORITY_INL
#define _RTEMS_SCORE_SCHEDULERPRIORITY_INL

/**
 *  @addtogroup ScoreScheduler
 * @{
 */

/** @brief  Scheduler priority Ready queue initialize
 *
 *  This routine initializes @a the_ready_queue for priority-based scheduling.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_initialize(void)
{
  size_t index;

  /* allocate ready queue structures */
  _Scheduler.Ready_queues.priority = (Chain_Control *) 
    _Workspace_Allocate_or_fatal_error(
      ((size_t) PRIORITY_MAXIMUM + 1) * sizeof(Chain_Control)
    );

  /* initialize ready queue structures */
  for( index=0; index <= PRIORITY_MAXIMUM; index++)
    _Chain_Initialize_empty( &_Scheduler.Ready_queues.priority[index] );
}

/**
 *  @brief _Scheduler_priority_Ready_queue_enqueue
 *
 *  This routine puts @a the_thread on to the priority-based ready queue.
 *  
 *  @param[in] the_thread  - pointer to thread
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_enqueue(
  Thread_Control                  *the_thread
)
{
  _Priority_bit_map_Add( &the_thread->scheduler.priority->Priority_map );
  
  _Chain_Append_unprotected( the_thread->scheduler.priority->ready_chain, 
      &the_thread->Object.Node );
}

/**
 *  @brief _Scheduler_priority_Ready_queue_Enqueue_first
 *
 *  This routine puts @a the_thread to the head of the ready queue. 
 *  For priority-based ready queues, the thread will be the first thread
 *  at its priority level.
 *  
 *  @param[in] the_thread  - pointer to thread
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_enqueue_first(
  Thread_Control                   *the_thread
)
{
  _Priority_bit_map_Add( &the_thread->scheduler.priority->Priority_map );

  _Chain_Prepend_unprotected( the_thread->scheduler.priority->ready_chain, 
      &the_thread->Object.Node );
}

/**
 *  @brief _Scheduler_priority_Ready_queue_extract
 *
 *  This routine removes a specific thread from the specified 
 *  priority-based ready queue.
 *
 *  @param[in] the_thread  - pointer to thread
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_extract(
  Thread_Control        *the_thread
)
{
  Chain_Control         *ready  = the_thread->scheduler.priority->ready_chain;

  if ( _Chain_Has_only_one_node( ready ) ) {
    _Chain_Initialize_empty( ready );
    _Priority_bit_map_Remove( &the_thread->scheduler.priority->Priority_map );
  } else
    _Chain_Extract_unprotected( &the_thread->Object.Node );
}

/**
 *  @brief _Scheduler_priority_Ready_queue_first
 *
 *  This routines returns a pointer to the first thread on @a the_ready_queue.
 *
 *  @param[in] the_ready_queue - pointer to thread queue
 *
 *  @return This method returns the first thread or NULL
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_priority_Ready_queue_first(
  Chain_Control       *the_ready_queue
)
{
  Priority_Control index = _Priority_bit_map_Get_highest();

  if ( !_Chain_Is_empty( &the_ready_queue[ index ] ) )
    return (Thread_Control *) _Chain_First( &the_ready_queue[ index ] );

  return NULL;
}

/**
 *  @brief _Scheduler_priority_Ready_queue_requeue
 *
 *  This routine is invoked when a thread changes priority and should be
 *  moved to a different position on the ready queue.
 *
 *  @param[in] the_thread  - pointer to thread
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Ready_queue_requeue(
  Thread_Control            *the_thread
)
{
  if ( !_Chain_Has_only_one_node(
        the_thread->scheduler.priority->ready_chain
        ) ) {
    _Chain_Extract_unprotected( &the_thread->Object.Node );

    _Chain_Append_unprotected( the_thread->scheduler.priority->ready_chain, 
      &the_thread->Object.Node );
  }
}

/**
 *  @brief _Scheduler_priority_Schedule_body
 *
 *  This kernel routine implements scheduling decision logic
 *  for priority-based scheduling.  
 *
 *  @param[in] the_thread  - pointer to thread
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Schedule_body(void)
{
  _Thread_Heir = _Scheduler_priority_Ready_queue_first(
      _Scheduler.Ready_queues.priority
  );
}

/**
 *  @brief _Scheduler_priority_Block_body
 *
 *  This kernel routine removes the_thread from scheduling decisions based 
 * on simple queue extraction.
 *
 *  @param[in] the_thread  - pointer to thread
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Block_body(
  Thread_Control   *the_thread
)
{
  _Scheduler_priority_Ready_queue_extract(the_thread);

  /* TODO: flash critical section */

  if ( _Thread_Is_heir( the_thread ) )
     _Scheduler_priority_Schedule_body();

  if ( _Thread_Is_executing( the_thread ) )
    _Thread_Dispatch_necessary = true;

  return;
}

/**
 *  @brief _Scheduler_priority_Unblock_body
 *
 *  This kernel routine readies the requested thread according to the queuing 
 *  discipline. A new heir thread may be selected.
 *
 *  @param[in] the_thread  - pointer to thread
 *
 *  @note This routine uses the "blocking" heir selection mechanism.
 *        This ensures the correct heir after a thread restart.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_priority_Unblock_body (
  Thread_Control          *the_thread
)
{
  _Scheduler_priority_Ready_queue_enqueue(the_thread);

  /* TODO: flash critical section */

  /*
   *  If the thread that was unblocked is more important than the heir,
   *  then we have a new heir.  This may or may not result in a
   *  context switch.
   *
   *  Normal case:
   *    If the current thread is preemptible, then we need to do
   *    a context switch.
   *  Pseudo-ISR case:
   *    Even if the thread isn't preemptible, if the new heir is
   *    a pseudo-ISR system task, we need to do a context switch.
   */
  if ( the_thread->current_priority < _Thread_Heir->current_priority ) {
    _Thread_Heir = the_thread;
    if ( _Thread_Executing->is_preemptible ||
        the_thread->current_priority == 0 )
      _Thread_Dispatch_necessary = true;
  }
}

/**@}*/

#endif
/* end of include file */
