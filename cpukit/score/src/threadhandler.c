/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

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

/*PAGE
 *
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
 *  Currently this is only really needed for the posix port,
 *  ref: _Context_Switch in unix/cpu.c
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _Thread_Handler( void )
{
  ISR_Level  level;
  Thread_Control *executing;
#ifdef USE_INIT_FINI
  static char doneConstructors;
  char doneCons;
#endif
 
  executing = _Thread_Executing;
 
  /*
   * have to put level into a register for those cpu's that use
   * inline asm here
   */
 
  level = executing->Start.isr_level;
  _ISR_Set_level(level);

#ifdef USE_INIT_FINI
  doneCons = doneConstructors;
  doneConstructors = 1;
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
#ifdef USE_INIT_FINI
  if (!doneCons)
    _init ();
#endif
 
  switch ( executing->Start.prototype ) {
    case THREAD_START_NUMERIC:
      executing->Wait.return_argument = 
        (*(Thread_Entry_numeric) executing->Start.entry_point)(
          executing->Start.numeric_argument
      );
      break;
    case THREAD_START_POINTER:
      executing->Wait.return_argument =
        (*(Thread_Entry_pointer) executing->Start.entry_point)(
          executing->Start.pointer_argument
        );
      break;
    case THREAD_START_BOTH_POINTER_FIRST:
      executing->Wait.return_argument = 
         (*(Thread_Entry_both_pointer_first) executing->Start.entry_point)( 
           executing->Start.pointer_argument,
           executing->Start.numeric_argument
         );
      break;
    case THREAD_START_BOTH_NUMERIC_FIRST:
      executing->Wait.return_argument = 
         (*(Thread_Entry_both_numeric_first) executing->Start.entry_point)( 
           executing->Start.numeric_argument,
           executing->Start.pointer_argument
         );
      break;
  }

  /*
   *  In the switch above, the return code from the user thread body
   *  was placed in return_argument.  This assumed that if it returned
   *  anything (which is not supporting in all APIs), then it would be 
   *  able to fit in a (void *).
   */

  _User_extensions_Thread_exitted( executing );

  _Internal_error_Occurred(
    INTERNAL_ERROR_CORE,
    TRUE,
    INTERNAL_ERROR_THREAD_EXITTED
  );
}
