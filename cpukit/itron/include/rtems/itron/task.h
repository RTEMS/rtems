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

#ifndef __RTEMS_ITRON_TASK_h_
#define __RTEMS_ITRON_TASK_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/itron/object.h>
#include <rtems/itron/config.h>
#include <rtems/score/thread.h>

/*
 *  This macro evaluates to TRUE if there should be a "context error."
 *  This is defined as a blocking directive being issed from 
 *  task-independent portions or a task in dispatch disabled state.
 *
 *  NOTE: Dispatching is disabled in interrupts so this should cover
 *        both cases.
 */

#define _ITRON_Is_in_non_task_state() \
  (_Thread_Dispatch_disable_level != 0)
 
/*
 *  This is the API specific information required by each thread for
 *  the ITRON API to function correctly.
 */


typedef struct {
  unsigned32                unused;  /* no use for the API extension */
                                     /*   structure for ITRON identified yet */ 
}  ITRON_API_Control;

/*
 *  The following defines the information control block used to
 *  manage this class of objects.
 */

ITRON_EXTERN Objects_Information _ITRON_Task_Information;

/*
 *  These are used to manage the user initialization tasks.
 */

ITRON_EXTERN itron_initialization_tasks_table
               *_ITRON_Task_User_initialization_tasks;
ITRON_EXTERN unsigned32   _ITRON_Task_Number_of_initialization_tasks;


/*
 *  _ITRON_Task_Manager_initialization
 *
 *  This routine initializes the ITRON Task Manager.
 */

void _ITRON_Task_Manager_initialization(
  unsigned32                        maximum_tasks,
  unsigned32                        number_of_initialization_tasks,
  itron_initialization_tasks_table *user_tasks
);

/*
 *
 *  _ITRON_Delete_task
 */

ER _ITRON_Delete_task(
  Thread_Control *the_thread
);

/* XXX remove the need for this.  Enable dispatch should not be hidden */

#define _ITRON_return_errorno( _errno )                \
do {                                                   \
  _Thread_Enable_dispatch();                           \
  return _errno;                                       \
} while (0);


#ifndef __RTEMS_APPLICATION__
#include <rtems/itron/task.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

