/*
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

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

/*
 * XXX - How do I return these errors ???  Do I have to validate the ID
         prior to calling the ID routine ??
  E_NOMEM   Insufficient memory (Memory for control block and/or user stack
            cannot be allocated)
  E_ID      Invalid ID Number (tskid was invalid or could not be used)
  E_RSATR   Reserved attribute (tskatr was invalid or could not be used)
  E_OBJ     Invalid object state (a task of the same ID already exists)
  E_OACV    Object access violation (A tskid less than -4 was specified from
            a user task.  This is implementation dependent.)
  E_PAR     Parameter error (pk_ctsk, task, itskpri and/or stksz is invalid)
  EN_OBJNO  An object number which could not be accessed on the target node
            is specified.
  EN_CTXID  Specified an object on another node when the system call was
            issued from a task in dispatch disabled state or from a task-
            independent portion
  EN_PAR    A value outside the range supported by the target node and/or
            transmission packet format was specified as a parameter (a value
            outside supported range was specified for exinf, tskatr, task,
            itskpri and/or stksz)
 */

ER cre_tsk(
  ID      tskid,
  T_CTSK *pk_ctsk
)
{
  register Thread_Control     *the_thread;
  char                        *name = "trn";
  boolean                      status;
  Priority_Control             core_priority;  

  /*
   * Disable dispatching.
   */
  
  _Thread_Disable_dispatch();

  /*
   * allocate the thread.
   */

  the_thread = _ITRON_Task_Allocate( tskid );
  if ( !the_thread ) {
    ena_dsp();
    return _ITRON_Task_Clarify_allocation_id_error( tskid ); 
  }

  /*
   * XXX - FIX THE VARIABLES TO THE CORRECT VALUES!!!
   */

  /*
   *  Initialize the core thread for this task.
   */

  core_priority = _ITRON_Task_Priority_to_Core( pk_ctsk->itskpri );
  status = _Thread_Initialize(
    &_ITRON_Task_Information,
    the_thread, 
    NULL,
    pk_ctsk->stksz,
    TRUE,          /* XXX - All tasks FP ??? */
    core_priority,
    TRUE,
    THREAD_CPU_BUDGET_ALGORITHM_EXHAUST_TIMESLICE,
    NULL,        /* no budget algorithm callout */
    0,
    &name
  );

  if ( !status ) {
    _ITRON_Task_Free( the_thread );
    _Thread_Enable_dispatch();
    return -1;
#if (0)
/* XXX */
#endif
  }

#if (0)  /* XXX We have any thing else to set per API structure? */
  api = the_thread->API_Extensions[ THREAD_API_ITRON ];
  asr = &api->Signal;
 
  asr->is_enabled = FALSE;

  *id = the_thread->Object.id;
#endif

  /*
   *  This insures we evaluate the process-wide signals pending when we
   *  first run.
   *
   *  NOTE:  Since the thread starts with all unblocked, this is necessary.
   */

  the_thread->do_post_task_switch_extension = TRUE;

  the_thread->Start.entry_point = (Thread_Entry) pk_ctsk->task;

  _Thread_Enable_dispatch();
  return E_OK;
}

