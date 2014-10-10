/**
 *  @file
 *
 *  @brief Thread Handler
 *  @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/interr.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/userextimpl.h>

void _Thread_Handler( void )
{
  Thread_Control *executing = _Thread_Executing;
  ISR_Level       level;


  /*
   * Some CPUs need to tinker with the call frame or registers when the
   * thread actually begins to execute for the first time.  This is a
   * hook point where the port gets a shot at doing whatever it requires.
   */
  _Context_Initialization_at_thread_begin();

  #if !defined(RTEMS_SMP)
    /*
     * have to put level into a register for those cpu's that use
     * inline asm here
     */
    level = executing->Start.isr_level;
    _ISR_Set_level( level );
  #endif

  /*
   * Initialize the floating point context because we do not come
   * through _Thread_Dispatch on our first invocation. So the normal
   * code path for performing the FP context switch is not hit.
   */
  #if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
    #if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )
      if ( (executing->fp_context != NULL) &&
            !_Thread_Is_allocated_fp( executing ) ) {
        if ( _Thread_Allocated_fp != NULL )
          _Context_Save_fp( &_Thread_Allocated_fp->fp_context );
        _Thread_Allocated_fp = executing;
      }
    #endif
  #endif

  /*
   * Take care that 'begin' extensions get to complete before
   * 'switch' extensions can run.  This means must keep dispatch
   * disabled until all 'begin' extensions complete.
   */
  _User_extensions_Thread_begin( executing );

  /*
   *  At this point, the dispatch disable level BETTER be 1.
   */
  #if defined(RTEMS_SMP)
    {
      /*
       * On SMP we enter _Thread_Handler() with interrupts disabled and
       * _Thread_Dispatch() obtained the per-CPU lock for us.  We have to
       * release it here and set the desired interrupt level of the thread.
       */
      Per_CPU_Control *cpu_self = _Per_CPU_Get();

      _Assert( cpu_self->thread_dispatch_disable_level == 1 );
      _Assert( _ISR_Get_level() != 0 );

      _Thread_Debug_set_real_processor( executing, cpu_self );

      cpu_self->thread_dispatch_disable_level = 0;
      _Profiling_Thread_dispatch_enable( cpu_self, 0 );

      level = executing->Start.isr_level;
      _ISR_Set_level( level);

      /*
       * The thread dispatch level changed from one to zero.  Make sure we lose
       * no thread dispatch necessary update.
       */
      _Thread_Dispatch();
    }
  #else
    _Thread_Enable_dispatch();
  #endif

  /*
   *  RTEMS supports multiple APIs and each API can define a different
   *  thread/task prototype. The following code supports invoking the
   *  user thread entry point using the prototype expected.
   */
  if ( executing->Start.prototype == THREAD_START_NUMERIC ) {
    executing->Wait.return_argument =
      (*(Thread_Entry_numeric) executing->Start.entry_point)(
        executing->Start.numeric_argument
      );
  }
  #if defined(RTEMS_POSIX_API)
    else if ( executing->Start.prototype == THREAD_START_POINTER ) {
      executing->Wait.return_argument =
        (*(Thread_Entry_pointer) executing->Start.entry_point)(
          executing->Start.pointer_argument
        );
    }
  #endif
  #if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
    else if ( executing->Start.prototype == THREAD_START_BOTH_POINTER_FIRST ) {
      executing->Wait.return_argument =
         (*(Thread_Entry_both_pointer_first) executing->Start.entry_point)(
           executing->Start.pointer_argument,
           executing->Start.numeric_argument
         );
    }
    else if ( executing->Start.prototype == THREAD_START_BOTH_NUMERIC_FIRST ) {
      executing->Wait.return_argument =
       (*(Thread_Entry_both_numeric_first) executing->Start.entry_point)(
         executing->Start.numeric_argument,
         executing->Start.pointer_argument
       );
    }
  #endif

  /*
   *  In the switch above, the return code from the user thread body
   *  was placed in return_argument.  This assumed that if it returned
   *  anything (which is not supporting in all APIs), then it would be
   *  able to fit in a (void *).
   */

  _User_extensions_Thread_exitted( executing );

  _Terminate(
    INTERNAL_ERROR_CORE,
    true,
    INTERNAL_ERROR_THREAD_EXITTED
  );
}
