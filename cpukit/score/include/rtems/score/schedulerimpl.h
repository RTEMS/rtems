/**
 * @file
 *
 * @brief Inlined Routines Associated with the Manipulation of the Scheduler
 *
 * This inline file contains all of the inlined routines associated with
 * the manipulation of the scheduler.
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERIMPL_H
#define _RTEMS_SCORE_SCHEDULERIMPL_H

#include <rtems/score/scheduler.h>
#include <rtems/score/threadimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreScheduler
 */
/**@{**/

/**
 *  @brief Initializes the scheduler to the policy chosen by the user.
 *
 *  This routine initializes the scheduler to the policy chosen by the user
 *  through confdefs, or to the priority scheduler with ready chains by
 *  default.
 */
void _Scheduler_Handler_initialization( void );

/**
 * The preferred method to add a new scheduler is to define the jump table
 * entries and add a case to the _Scheduler_Initialize routine.
 *
 * Generic scheduling implementations that rely on the ready queue only can
 * be found in the _Scheduler_queue_XXX functions.
 */

/*
 * Passing the Scheduler_Control* to these functions allows for multiple
 * scheduler's to exist simultaneously, which could be useful on an SMP
 * system.  Then remote Schedulers may be accessible.  How to protect such
 * accesses remains an open problem.
 */

/**
 * @brief Scheduler schedule.
 *
 * This kernel routine implements the scheduling decision logic for
 * the scheduler. It does NOT dispatch.
 *
 * @param[in] thread The thread which state changed previously.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Schedule( Thread_Control *thread )
{
  _Scheduler.Operations.schedule( thread );
}

/**
 * @brief Scheduler yield with a particular thread.
 *
 * This routine is invoked when a thread wishes to voluntarily transfer control
 * of the processor to another thread.
 *
 * @param[in] thread The yielding thread.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Yield(
  Thread_Control *thread
)
{
  ( *_Scheduler.Operations.yield )( thread );
}

/**
 * @brief Scheduler block.
 *
 * This routine removes @a the_thread from the scheduling decision for
 * the scheduler. The primary task is to remove the thread from the
 * ready queue.  It performs any necessary schedulering operations
 * including the selection of a new heir thread.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Block(
    Thread_Control    *the_thread
)
{
  _Scheduler.Operations.block( the_thread );
}

/**
 * @brief Scheduler unblock.
 *
 * This routine adds @a the_thread to the scheduling decision for
 * the scheduler.  The primary task is to add the thread to the
 * ready queue per the schedulering policy and update any appropriate
 * scheduling variables, for example the heir thread.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Unblock(
    Thread_Control    *the_thread
)
{
  _Scheduler.Operations.unblock( the_thread );
}

/**
 * @brief Scheduler allocate.
 *
 * This routine allocates @a the_thread->scheduler
 */
RTEMS_INLINE_ROUTINE void* _Scheduler_Allocate(
  Thread_Control    *the_thread
)
{
  return _Scheduler.Operations.allocate( the_thread );
}

/**
 * @brief Scheduler free.
 *
 * This routine frees @a the_thread->scheduler
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Free(
  Thread_Control    *the_thread
)
{
  _Scheduler.Operations.free( the_thread );
}

/**
 * @brief Scheduler update.
 *
 * This routine updates @a the_thread->scheduler
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Update(
  Thread_Control    *the_thread
)
{
  _Scheduler.Operations.update( the_thread );
}

/**
 * @brief Scheduler enqueue.
 *
 * This routine enqueue @a the_thread->scheduler
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Enqueue(
  Thread_Control    *the_thread
)
{
  _Scheduler.Operations.enqueue( the_thread );
}

/**
 * @brief Scheduler enqueue first.
 *
 * This routine enqueue_first @a the_thread->scheduler
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Enqueue_first(
  Thread_Control    *the_thread
)
{
  _Scheduler.Operations.enqueue_first( the_thread );
}

/**
 * @brief Scheduler extract.
 *
 * This routine extract @a the_thread->scheduler
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Extract(
  Thread_Control    *the_thread
)
{
  _Scheduler.Operations.extract( the_thread );
}

/**
 * @brief Scheduler priority compare.
 *
 * This routine compares two priorities.
 */
RTEMS_INLINE_ROUTINE int _Scheduler_Priority_compare(
  Priority_Control p1,
  Priority_Control p2
)
{
  return _Scheduler.Operations.priority_compare(p1, p2);
}

/**
 * @brief Scheduler release job.
 *
 * This routine is called when a new period of task is issued.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Release_job(
  Thread_Control *the_thread,
  uint32_t       length
)
{
  _Scheduler.Operations.release_job(the_thread, length);
}

/**
 * @brief Scheduler method invoked at each clock tick.
 *
 * This method is invoked at each clock tick to allow the scheduler
 * implementation to perform any activities required.  For the
 * scheduler which support standard RTEMS features, this includes
 * time-slicing management.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Tick( void )
{
  _Scheduler.Operations.tick();
}

/**
 * @brief Starts the idle thread for a particular processor.
 *
 * @param[in,out] thread The idle thread for the processor.
 * @parma[in,out] processor The processor for the idle thread.
 *
 * @see _Thread_Create_idle().
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Start_idle(
  Thread_Control *thread,
  Per_CPU_Control *processor
)
{
  ( *_Scheduler.Operations.start_idle )( thread, processor );
}

RTEMS_INLINE_ROUTINE void _Scheduler_Update_heir(
  Thread_Control *heir,
  bool force_dispatch
)
{
  Thread_Control *executing = _Thread_Executing;

  _Thread_Heir = heir;

  if ( executing != heir && ( force_dispatch || executing->is_preemptible ) )
    _Thread_Dispatch_necessary = true;
}

RTEMS_INLINE_ROUTINE void _Scheduler_Generic_block(
  void ( *extract )( Thread_Control *thread ),
  void ( *schedule )( Thread_Control *thread, bool force_dispatch ),
  Thread_Control *thread
)
{
  ( *extract )( thread );

  /* TODO: flash critical section? */

  if ( _Thread_Is_executing( thread ) || _Thread_Is_heir( thread ) ) {
    ( *schedule )( thread, true );
  }
}

/**
 * Macro testing whether @a p1 has lower priority than @a p2
 * in the intuitive sense of priority.
 */
#define _Scheduler_Is_priority_lower_than( _p1, _p2 ) \
  (_Scheduler_Priority_compare(_p1,_p2) < 0)

/**
 * Macro testing whether @a p1 has higher priority than @a p2
 * in the intuitive sense of priority.
 */
#define _Scheduler_Is_priority_higher_than( _p1, _p2 ) \
  (_Scheduler_Priority_compare(_p1,_p2) > 0)

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
