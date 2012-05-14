/*
 *  Thread Handler
 *
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
#include <rtems/score/apiext.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>

#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  #include <rtems/score/timestamp.h>
#endif

#if defined(RTEMS_SMP)
  #include <rtems/score/smp.h>
#endif

/**
 *  _Thread_Dispatch
 *
 *  This kernel routine determines if a dispatch is needed, and if so
 *  dispatches to the heir thread.  Once the heir is running an attempt
 *  is made to dispatch any ASRs.
 *
 *  ALTERNATE ENTRY POINTS:
 *    void _Thread_Enable_dispatch();
 *
 *  INTERRUPT LATENCY:
 *    dispatch thread
 *    no dispatch thread
 */
void _Thread_Dispatch( void )
{
  Thread_Control   *executing;
  Thread_Control   *heir;
  ISR_Level         level;

  _Thread_Disable_dispatch();
  #if defined(RTEMS_SMP)
    /*
     *  If necessary, send dispatch request to other cores.
     */
    _SMP_Request_other_cores_to_dispatch();
  #endif

  /*
   *  Now determine if we need to perform a dispatch on the current CPU.
   */
  executing   = _Thread_Executing;
  _ISR_Disable( level );
  while ( _Thread_Dispatch_necessary == true ) {

    heir = _Thread_Heir;
    _Thread_Dispatch_necessary = false;
    _Thread_Executing = heir;

    /*
     *  When the heir and executing are the same, then we are being
     *  requested to do the post switch dispatching.  This is normally
     *  done to dispatch signals.
     */
    if ( heir == executing )
      goto post_switch;

    /*
     *  Since heir and executing are not the same, we need to do a real
     *  context switch.
     */
#if __RTEMS_ADA__
    executing->rtems_ada_self = rtems_ada_self;
    rtems_ada_self = heir->rtems_ada_self;
#endif
    if ( heir->budget_algorithm == THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE )
      heir->cpu_time_budget = _Thread_Ticks_per_timeslice;

    _ISR_Enable( level );

    #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
      {
        Timestamp_Control uptime, ran;
        _TOD_Get_uptime( &uptime );
        _Timestamp_Subtract(
          &_Thread_Time_of_last_context_switch,
          &uptime,
          &ran
        );
        _Timestamp_Add_to( &executing->cpu_time_used, &ran );
        _Thread_Time_of_last_context_switch = uptime;
      }
    #else
      {
        TOD_Get_uptime( &_Thread_Time_of_last_context_switch );
        heir->cpu_time_used++;
      }
    #endif

    /*
     * Switch libc's task specific data.
     */
    if ( _Thread_libc_reent ) {
      executing->libc_reent = *_Thread_libc_reent;
      *_Thread_libc_reent = heir->libc_reent;
    }

    _User_extensions_Thread_switch( executing, heir );

    /*
     *  If the CPU has hardware floating point, then we must address saving
     *  and restoring it as part of the context switch.
     *
     *  The second conditional compilation section selects the algorithm used
     *  to context switch between floating point tasks.  The deferred algorithm
     *  can be significantly better in a system with few floating point tasks
     *  because it reduces the total number of save and restore FP context
     *  operations.  However, this algorithm can not be used on all CPUs due
     *  to unpredictable use of FP registers by some compilers for integer
     *  operations.
     */

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
#if ( CPU_USE_DEFERRED_FP_SWITCH != TRUE )
    if ( executing->fp_context != NULL )
      _Context_Save_fp( &executing->fp_context );
#endif
#endif

    _Context_Switch( &executing->Registers, &heir->Registers );

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
#if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )
    if ( (executing->fp_context != NULL) &&
         !_Thread_Is_allocated_fp( executing ) ) {
      if ( _Thread_Allocated_fp != NULL )
        _Context_Save_fp( &_Thread_Allocated_fp->fp_context );
      _Context_Restore_fp( &executing->fp_context );
      _Thread_Allocated_fp = executing;
    }
#else
    if ( executing->fp_context != NULL )
      _Context_Restore_fp( &executing->fp_context );
#endif
#endif

    executing = _Thread_Executing;

    _ISR_Disable( level );
  }

post_switch:

  _ISR_Enable( level );

  _Thread_Unnest_dispatch();
 
  _API_extensions_Run_postswitch();
}
