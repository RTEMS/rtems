/** 
 *  @file  rtems/score/scheduler.inl
 *
 *  This inline file contains all of the inlined routines associated with
 *  the manipulation of the scheduler.
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULER_H
# error "Never use <rtems/score/scheduler.inl> directly; include <rtems/score/scheduler.h> instead."
#endif

#ifndef _RTEMS_SCORE_SCHEDULER_INL
#define _RTEMS_SCORE_SCHEDULER_INL

/**
 *  @addtogroup ScoreScheduler
 * @{
 */

/**
 * The preferred method to add a new scheduler is to define the jump table 
 * entries and add a case to the _Scheduler_Initialize routine. 
 *
 * Generic scheduling implementations that rely on the ready queue only can 
 * be found in the _Scheduler_queue_XXX functions.
 *
 */

/* Passing the Scheduler_Control* to these functions allows for multiple 
 * scheduler's to exist simultaneously, which could be useful on an SMP 
 * system.  Then remote Schedulers may be accessible.  How to protect such 
 * accesses remains an open problem.
 */

/** @brief _Scheduler_Schedule
 *
 *  This kernel routine implements the scheduling decision logic for 
 *  the scheduler. It does NOT dispatch.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Schedule( void )
{
  _Scheduler.Operations.schedule();
}

/** @brief _Scheduler_Yield
 *
 *  This routine is invoked when a thread wishes to voluntarily
 *  transfer control of the processor to another thread. This routine
 *  always operates on the scheduler that 'owns' the currently executing
 *  thread.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Yield( void )
{
  _Scheduler.Operations.yield();
}

/** @brief _Scheduler_Block
 *
 *  This routine removes @a the_thread from the scheduling decision for 
 *  the scheduler. The primary task is to remove the thread from the 
 *  ready queue.  It performs any necessary schedulering operations 
 *  including the selection of a new heir thread.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Block( 
    Thread_Control    *the_thread 
)
{
  _Scheduler.Operations.block( the_thread );
}

/** @brief _Scheduler_Unblock
 *
 *  This routine adds @a the_thread to the scheduling decision for 
 *  the scheduler.  The primary task is to add the thread to the
 *  ready queue per the schedulering policy and update any appropriate 
 *  scheduling variables, for example the heir thread.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Unblock(
    Thread_Control    *the_thread 
)
{
  _Scheduler.Operations.unblock( the_thread );
}

/** @brief _Scheduler_Allocate
 *
 * This routine allocates @a the_thread->scheduler
 */
RTEMS_INLINE_ROUTINE void* _Scheduler_Allocate( 
  Thread_Control    *the_thread
)
{
  return _Scheduler.Operations.allocate( the_thread );
}

/** @brief _Scheduler_Free
 *
 * This routine frees @a the_thread->scheduler
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Free( 
  Thread_Control    *the_thread
)
{
  return _Scheduler.Operations.free( the_thread );
}

/** @brief _Scheduler_Update
 *
 * This routine updates @a the_thread->scheduler
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Update( 
  Thread_Control    *the_thread
)
{
  _Scheduler.Operations.update( the_thread );
}

/** @brief _Scheduler_Enqueue
 *
 * This routine enqueue @a the_thread->scheduler
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Enqueue( 
  Thread_Control    *the_thread
)
{
  _Scheduler.Operations.enqueue( the_thread );
}

/** @brief _Scheduler_Enqueue_first
 *
 * This routine enqueue_first @a the_thread->scheduler
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Enqueue_first( 
  Thread_Control    *the_thread
)
{
  _Scheduler.Operations.enqueue_first( the_thread );
}

/** @brief _Scheduler_Extract
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
 * @brief Scheduler Priority compare
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
 * @brief Scheduler Release job
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

/** @brief Scheduler Method Invoked at Each Clock Tick
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

/**@}*/

#endif
/* end of include file */
