/**
 * @file
 *
 * @brief Default Scheduler At Tick Handler
 *
 * @ingroup ScoreScheduler
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/thread.h>
#include <rtems/score/smp.h>

static void _Scheduler_default_Tick_for_executing( Thread_Control *executing )
{
  #ifdef __RTEMS_USE_TICKS_FOR_STATISTICS__
    /*
     *  Increment the number of ticks this thread has been executing
     */
    executing->cpu_time_used++;
  #endif

  /*
   *  If the thread is not preemptible or is not ready, then
   *  just return.
   */

  if ( !executing->is_preemptible )
    return;

  if ( !_States_Is_ready( executing->current_state ) )
    return;

  /*
   *  The cpu budget algorithm determines what happens next.
   */

  switch ( executing->budget_algorithm ) {
    case THREAD_CPU_BUDGET_ALGORITHM_NONE:
      break;

    case THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE:
    #if defined(RTEMS_SCORE_THREAD_ENABLE_EXHAUST_TIMESLICE)
      case THREAD_CPU_BUDGET_ALGORITHM_EXHAUST_TIMESLICE:
    #endif
      if ( (int)(--executing->cpu_time_budget) <= 0 ) {

        /*
         *  A yield performs the ready chain mechanics needed when
         *  resetting a timeslice.  If no other thread's are ready
         *  at the priority of the currently executing thread, then the
         *  executing thread's timeslice is reset.  Otherwise, the
         *  currently executing thread is placed at the rear of the
         *  FIFO for this priority and a new heir is selected.
         */
        _Scheduler_Yield( executing );
        executing->cpu_time_budget = _Thread_Ticks_per_timeslice;
      }
      break;

    #if defined(RTEMS_SCORE_THREAD_ENABLE_SCHEDULER_CALLOUT)
      case THREAD_CPU_BUDGET_ALGORITHM_CALLOUT:
	if ( --executing->cpu_time_budget == 0 )
	  (*executing->budget_callout)( executing );
	break;
    #endif
  }
}

void _Scheduler_default_Tick( void )
{
  uint32_t processor_count = _SMP_Get_processor_count();
  uint32_t processor;

  for ( processor = 0 ; processor < processor_count ; ++processor ) {
    _Scheduler_default_Tick_for_executing(
      _Per_CPU_Information[ processor ].executing
    );
  }
}
