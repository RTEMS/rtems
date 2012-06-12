/**
 *  @file
 *
 *  Initialization Wrapper for all Threads.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
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
#if defined(RTEMS_SMP)
  #include <rtems/score/smp.h>
#endif

/*
 *  Conditional magic to determine what style of C++ constructor
 *  initialization this target and compiler version uses.
 */
#if defined(__USE_INIT_FINI__)
  #if defined(__M32R__)
    #define INIT_NAME __init
  #elif defined(__ARM_EABI__)
    #define INIT_NAME __libc_init_array
  #else
    #define INIT_NAME _init
  #endif

  extern void INIT_NAME(void);
  #define EXECUTE_GLOBAL_CONSTRUCTORS
#endif

#if defined(__USE__MAIN__)
  extern void __main(void);
  #define INIT_NAME __main
  #define EXECUTE_GLOBAL_CONSTRUCTORS
#endif

/*
 *  _Thread_Handler
 *
 *  This routine is the "primal" entry point for all threads.
 *  _Context_Initialize() dummies up the thread's initial context
 *  to cause the first Context_Switch() to jump to _Thread_Handler().
 *
 *  This routine is the default thread exitted error handler.  It is
 *  returned to when a thread exits.  The configured fatal error handler
 *  is invoked to process the exit.
 *
 *  NOTE:
 *
 *  On entry, it is assumed all interrupts are blocked and that this
 *  routine needs to set the initial isr level.  This may or may not
 *  actually be needed by the context switch routine and as a result
 *  interrupts may already be at there proper level.  Either way,
 *  setting the initial isr level properly here is safe.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */
void _Thread_Handler( void )
{
  ISR_Level  level;
  Thread_Control *executing;
  #if defined(EXECUTE_GLOBAL_CONSTRUCTORS)
    static bool doneConstructors;
    bool doCons;
  #endif

  executing = _Thread_Executing;

  /*
   * Some CPUs need to tinker with the call frame or registers when the
   * thread actually begins to execute for the first time.  This is a
   * hook point where the port gets a shot at doing whatever it requires.
   */
  _Context_Initialization_at_thread_begin();

  /*
   * have to put level into a register for those cpu's that use
   * inline asm here
   */
  level = executing->Start.isr_level;
  _ISR_Set_level(level);

  #if defined(EXECUTE_GLOBAL_CONSTRUCTORS)
    #if defined(RTEMS_MULTIPROCESSING)
      doCons = !doneConstructors
        && _Objects_Get_API( executing->Object.id ) != OBJECTS_INTERNAL_API;
      if (doCons)
        doneConstructors = true;
    #else
      doCons = !doneConstructors;
      doneConstructors = true;
    #endif
  #endif

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
  _Thread_Enable_dispatch();

  #if defined(EXECUTE_GLOBAL_CONSTRUCTORS)
    /*
     *  _init could be a weak symbol and we SHOULD test it but it isn't
     *  in any configuration I know of and it generates a warning on every
     *  RTEMS target configuration.  --joel (12 May 2007)
     */
    if (doCons) /* && (volatile void *)_init) */ {
      INIT_NAME ();
   
      #if defined(RTEMS_SMP)
        _Thread_Disable_dispatch();
          _SMP_Request_other_cores_to_perform_first_context_switch();
        _Thread_Enable_dispatch();
      #endif
    }
 #endif

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

  _Internal_error_Occurred(
    INTERNAL_ERROR_CORE,
    true,
    INTERNAL_ERROR_THREAD_EXITTED
  );
}
