/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
 *  ref_tsk - Reference Task Status
 */

ER ref_tsk(
  T_RTSK *pk_rtsk,
  ID      tskid
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;
  Priority_Control         core_priority;  

  if (!pk_rtsk)
    return E_PAR;

  the_thread = _ITRON_Task_Get( tskid, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:
      return _ITRON_Task_Clarify_get_id_error( tskid );
  
    case OBJECTS_LOCAL:

      if ( location != OBJECTS_LOCAL )
        return  _ITRON_Task_Clarify_get_id_error( tskid );

      /*
       * The following are extended functions [level X ].
       * XXX - tskwait, wid, wupcnt, and tskatr are presently not implemented.
       */

      pk_rtsk->tskwait = 0;
      pk_rtsk->wid     = 0;
      pk_rtsk->wupcnt  = 0;
      pk_rtsk->suscnt  = the_thread->suspend_count;
      pk_rtsk->tskatr  = 0;       /* XXX - Not correctly implemented */
      pk_rtsk->task    = (FP) the_thread->Start.entry_point; 
      core_priority    = the_thread->Start.initial_priority;
      pk_rtsk->itskpri = _ITRON_Task_Core_to_Priority( core_priority );
      pk_rtsk->stksz   = the_thread->Start.Initial_stack.size;

      /*
       * The following are required.
       */

      pk_rtsk->exinf   = NULL;   /* extended information */
      pk_rtsk->tskpri  =
                    _ITRON_Task_Core_to_Priority(the_thread->current_priority);

      /*
       * Mask in the tskstat information
       * Convert the task state XXX double check this
       */

      pk_rtsk->tskstat = 0;
      if ( the_thread == _Thread_Executing )
        pk_rtsk->tskstat |= TTS_RUN;
      if ( _States_Is_ready(the_thread->current_state) )
        pk_rtsk->tskstat |= TTS_RDY;  
      if ( _States_Is_dormant( the_thread->current_state) )
        pk_rtsk->tskstat |= TTS_DMT;
      if ( _States_Is_suspended(the_thread->current_state) )
        pk_rtsk->tskstat |= TTS_SUS;
      if ( _States_Is_blocked(the_thread->current_state) )
        pk_rtsk->tskstat |= TTS_WAI;

      break;
  }

  _ITRON_return_errorno( E_OK );

}




