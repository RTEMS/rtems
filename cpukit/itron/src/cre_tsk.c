/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <itron.h>

#include <rtems/score/thread.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>

#include <rtems/itron/task.h>


/*
 *  cre_tsk - Create Task
 */

ER cre_tsk(
  ID      tskid,
  T_CTSK *pk_ctsk
)
{
  register Thread_Control     *the_thread;
  boolean                      status;
  Priority_Control             core_priority;  

  /*
   * Validate Parameters.
   */
  
 if ( pk_ctsk == NULL )
    return E_PAR;

  if ((pk_ctsk->tskatr != TA_ASM ) &&
      (pk_ctsk->tskatr != TA_HLNG) &&
      (pk_ctsk->tskatr != TA_COP0) &&
      (pk_ctsk->tskatr != TA_COP1) &&
      (pk_ctsk->tskatr != TA_COP2) &&
      (pk_ctsk->tskatr != TA_COP3) &&
      (pk_ctsk->tskatr != TA_COP4) &&
      (pk_ctsk->tskatr != TA_COP5) &&
      (pk_ctsk->tskatr != TA_COP6) &&
      (pk_ctsk->tskatr != TA_COP7))
    return E_RSATR;

  if (( pk_ctsk->itskpri <= 0 ) || ( pk_ctsk->itskpri >= 256 ))
    return E_PAR;
  if ( pk_ctsk->task == NULL )
    return E_PAR;
  if ( pk_ctsk->stksz < 0 )
    return E_PAR;
  
  /*
   * Disable dispatching.
   */
  
  _Thread_Disable_dispatch();

  /*
   * allocate the thread.
   */

  the_thread = _ITRON_Task_Allocate( tskid );
  if ( !the_thread )
    _ITRON_return_errorno( _ITRON_Task_Clarify_allocation_id_error( tskid ) );

  /*
   *  Initialize the core thread for this task.
   */

  core_priority = _ITRON_Task_Priority_to_Core( pk_ctsk->itskpri );
  status = _Thread_Initialize(
    &_ITRON_Task_Information,
    the_thread, 
    NULL,
    pk_ctsk->stksz,
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
    TRUE,          /* XXX - All tasks FP (if the HW supports it) for now */
#else
    FALSE,
#endif
    core_priority,
    TRUE,        /* preemptible */
    THREAD_CPU_BUDGET_ALGORITHM_EXHAUST_TIMESLICE,
    NULL,        /* no budget algorithm callout */
    0,
    NULL
  );

  if ( !status ) {
    _ITRON_Task_Free( the_thread );
    _ITRON_return_errorno( E_NOMEM );
  }

  /*
   *  This insures we evaluate the process-wide signals pending when we
   *  first run.
   *
   *  NOTE:  Since the thread starts with all unblocked, this is necessary.
   */

  the_thread->do_post_task_switch_extension = TRUE;

  the_thread->Start.entry_point = (Thread_Entry) pk_ctsk->task;

  _ITRON_return_errorno( E_OK );
}




