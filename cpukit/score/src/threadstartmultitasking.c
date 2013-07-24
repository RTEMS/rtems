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
#include <rtems/score/sysstate.h>

void _Thread_Start_multitasking( void )
{
  /*
   *  The system is now multitasking and completely initialized.
   *  This system thread now "hides" in a single processor until
   *  the system is shut down.
   */

  _System_state_Set( SYSTEM_STATE_UP );

  _Thread_Dispatch_necessary = false;

  #if defined(RTEMS_SMP)
    _Thread_Executing->is_executing = false;
    _Thread_Heir->is_executing = true;
  #endif

  _Thread_Executing = _Thread_Heir;

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

   if ( _Thread_Heir->fp_context != NULL )
     _Context_Restore_fp( &_Thread_Heir->fp_context );
#endif

#if defined(_CPU_Start_multitasking)
  _CPU_Start_multitasking( &_Thread_BSP_context, &_Thread_Heir->Registers );
#else
  _Context_Switch( &_Thread_BSP_context, &_Thread_Heir->Registers );
#endif
}
