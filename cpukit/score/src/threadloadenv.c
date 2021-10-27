/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains the implementation of
 *   _Thread_Load_environment().
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>

void _Thread_Load_environment(
  Thread_Control *the_thread
)
{
  const Thread_CPU_budget_operations *cpu_budget_operations;

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  if ( the_thread->Start.fp_context ) {
    the_thread->fp_context = the_thread->Start.fp_context;
    _Context_Initialize_fp( &the_thread->fp_context );
  }
#endif

  the_thread->is_preemptible   = the_thread->Start.is_preemptible;

  cpu_budget_operations = the_thread->Start.cpu_budget_operations;
  the_thread->CPU_budget.operations = cpu_budget_operations;

  if ( cpu_budget_operations != NULL ) {
    ( *cpu_budget_operations->initialize )( the_thread );
  }

  _Context_Initialize(
    &the_thread->Registers,
    the_thread->Start.Initial_stack.area,
    the_thread->Start.Initial_stack.size,
    the_thread->Start.isr_level,
    _Thread_Handler,
    the_thread->is_fp,
    the_thread->Start.tls_area
  );
}
