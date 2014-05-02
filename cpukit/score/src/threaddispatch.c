/**
 * @file
 * 
 * @brief Dispatch Thread
 * @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2014 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threaddispatch.h>
#include <rtems/score/apiext.h>
#include <rtems/score/assert.h>
#include <rtems/score/isr.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/config.h>

static Thread_Action *_Thread_Get_post_switch_action(
  Thread_Control *executing
)
{
  Chain_Control *chain = &executing->Post_switch_actions.Chain;

  return (Thread_Action *) _Chain_Get_unprotected( chain );
}

static void _Thread_Run_post_switch_actions( Thread_Control *executing )
{
  ISR_Level        level;
  Per_CPU_Control *cpu_self;
  Thread_Action   *action;

  cpu_self = _Thread_Action_ISR_disable_and_acquire( executing, &level );
  action = _Thread_Get_post_switch_action( executing );

  while ( action != NULL ) {
    _Chain_Set_off_chain( &action->Node );

    ( *action->handler )( executing, action, cpu_self, level );

    cpu_self = _Thread_Action_ISR_disable_and_acquire( executing, &level );
    action = _Thread_Get_post_switch_action( executing );
  }

  _Thread_Action_release_and_ISR_enable( cpu_self, level );
}

void _Thread_Dispatch( void )
{
  Per_CPU_Control  *cpu_self;
  Thread_Control   *executing;
  ISR_Level         level;

#if defined( RTEMS_SMP )
  /*
   * On SMP the complete context switch must be atomic with respect to one
   * processor.  See also _Thread_Handler() since _Context_switch() may branch
   * to this function.
   */
  _ISR_Disable_without_giant( level );
#endif

  cpu_self = _Per_CPU_Get();
  _Assert( cpu_self->thread_dispatch_disable_level == 0 );
  _Profiling_Thread_dispatch_disable( cpu_self, 0 );
  cpu_self->thread_dispatch_disable_level = 1;

  /*
   *  Now determine if we need to perform a dispatch on the current CPU.
   */
  executing = cpu_self->executing;

#if !defined( RTEMS_SMP )
  _ISR_Disable( level );
#endif

#if defined( RTEMS_SMP )
  if ( cpu_self->dispatch_necessary ) {
#else
  while ( cpu_self->dispatch_necessary ) {
#endif
    Thread_Control *heir = _Thread_Get_heir_and_make_it_executing( cpu_self );

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
      heir->cpu_time_budget = rtems_configuration_get_ticks_per_timeslice();

#if !defined( RTEMS_SMP )
    _ISR_Enable( level );
#endif

    #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
      _Thread_Update_cpu_time_used(
        executing,
        &cpu_self->time_of_last_context_switch
      );
    #else
      {
        _TOD_Get_uptime( &cpu_self->time_of_last_context_switch );
        heir->cpu_time_used++;
      }
    #endif

#if !defined(__DYNAMIC_REENT__)
    /*
     * Switch libc's task specific data.
     */
    if ( _Thread_libc_reent ) {
      executing->libc_reent = *_Thread_libc_reent;
      *_Thread_libc_reent = heir->libc_reent;
    }
#endif

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

    /*
     * We have to obtain this value again after the context switch since the
     * heir thread may have migrated from another processor.  Values from the
     * stack or non-volatile registers reflect the old execution environment.
     */
    cpu_self = _Per_CPU_Get();

    _Thread_Debug_set_real_processor( executing, cpu_self );

#if !defined( RTEMS_SMP )
    _ISR_Disable( level );
#endif
  }

post_switch:
  _Assert( cpu_self->thread_dispatch_disable_level == 1 );
  cpu_self->thread_dispatch_disable_level = 0;
  _Profiling_Thread_dispatch_enable( cpu_self, 0 );

  _ISR_Enable_without_giant( level );

  _Thread_Run_post_switch_actions( executing );
}
