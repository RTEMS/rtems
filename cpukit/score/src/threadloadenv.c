/**
 * @file
 *
 * @brief Initializes Enviroment for A Thread
 *
 * @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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

void _Thread_Load_environment(
  Thread_Control *the_thread
)
{
  bool is_fp;
  uint32_t isr_level;

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  if ( the_thread->Start.fp_context ) {
    the_thread->fp_context = the_thread->Start.fp_context;
    _Context_Initialize_fp( &the_thread->fp_context );
    is_fp = true;
  } else
#endif
    is_fp = false;

  the_thread->is_preemptible   = the_thread->Start.is_preemptible;
  the_thread->budget_algorithm = the_thread->Start.budget_algorithm;
  the_thread->budget_callout   = the_thread->Start.budget_callout;

#if defined( RTEMS_SMP )
  /*
   * On SMP we have to start the threads with interrupts disabled, see also
   * _Thread_Handler() and _Thread_Dispatch().  In _Thread_Handler() the
   * _ISR_Set_level() is used to set the desired interrupt state of the thread.
   */
  isr_level = CPU_MODES_INTERRUPT_MASK;
#else
  isr_level = the_thread->Start.isr_level;
#endif

  _Context_Initialize(
    &the_thread->Registers,
    the_thread->Start.Initial_stack.area,
    the_thread->Start.Initial_stack.size,
    isr_level,
    _Thread_Handler,
    is_fp
  );

}
