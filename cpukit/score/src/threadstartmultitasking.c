/**
 *  @file
 *
 *  @brief Start Thread Multitasking
 *  @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>

void _Thread_Start_multitasking( Context_Control *context )
{
  Per_CPU_Control *self_cpu = _Per_CPU_Get();
  Thread_Control  *heir = self_cpu->heir;

#if defined(RTEMS_SMP)
  _Per_CPU_Change_state( self_cpu, PER_CPU_STATE_UP );

  _Per_CPU_Acquire( self_cpu );

  self_cpu->executing->is_executing = false;
  heir->is_executing = true;
#endif

  self_cpu->dispatch_necessary = false;
  self_cpu->executing = heir;

   /*
    * Get the init task(s) running.
    *
    * Note: Thread_Dispatch() is normally used to dispatch threads.  As
    *       part of its work, Thread_Dispatch() restores floating point
    *       state for the heir task.
    *
    *       This code avoids Thread_Dispatch(), and so we have to restore
    *       (actually initialize) the floating point state "by hand".
    *
    *       Ignore the CPU_USE_DEFERRED_FP_SWITCH because we must always
    *       switch in the first thread if it is FP.
    */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
   /*
    *  don't need to worry about saving BSP's floating point state
    */

   if ( heir->fp_context != NULL )
     _Context_Restore_fp( &heir->fp_context );
#endif

#if defined(RTEMS_SMP)
  if ( context != NULL ) {
#endif

#if defined(_CPU_Start_multitasking)
    _CPU_Start_multitasking( context, &heir->Registers );
#else
    _Context_Switch( context, &heir->Registers );
#endif

#if defined(RTEMS_SMP)
  } else {
    /*
     * Threads begin execution in the _Thread_Handler() function.   This
     * function will set the thread dispatch disable level to zero and calls
     * _Per_CPU_Release().
     */
    self_cpu->thread_dispatch_disable_level = 1;

    _CPU_Context_switch_to_first_task_smp( &heir->Registers );
  }
#endif
}
