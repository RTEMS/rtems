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
 *  sta_tsk - Start Task
 */

/*
 * XXX - How Do I know when these happen ???
  E_NOEXS   Object does not exist (the task specified by tskid does not exist)
  E_OACV    Object access violation (A tskid less than -4 was specified from
            a user task.  This is implementation dependent.)
  E_OBJ     Invalid object state (the target task is not in DORMANT state)
  EN_OBJNO  An object number which could not be accessed on the target node
            is specified. XXX Should never get on a single processor??
  EN_CTXID  Specified an object on another node when the system call was
            issued from a task in dispatch disabled state or from a task-
            independent portionXXX Should never get on a single processor??
  EN_PAR    A value outside the range supported by the target node and/or
            transmission packet format was specified as a parameter (a value
            outside supported range was specified for stacd)
XXX- What does _ITRON_Task_Get return on an invalid id and how do you know
     if it is E_ID, E_NOEXS, E_OACV 
*/

ER sta_tsk(
  ID   tskid,
  INT  stacd
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;
  boolean                  status;

  the_thread = _ITRON_Task_Get( tskid, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
    case OBJECTS_ERROR:
      return E_ID;  /* XXX */

    case OBJECTS_LOCAL:
      status = _Thread_Start(
        the_thread,
        THREAD_START_NUMERIC, /* XXX should be able to say we have no arg */
        the_thread->Start.entry_point,
        0,                    /* XXX supercore forces us to have an arg */
        0                     /* unused */
      );

      /*
       *  Wrong state  XXX
       */

      if ( !status ) {
        _Thread_Enable_dispatch();
        return E_OBJ;
      }

      _Thread_Enable_dispatch();
      return E_OK;
  }

  return E_OBJ;   /* unreached - only to remove warnings */
}




