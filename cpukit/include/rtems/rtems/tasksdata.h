/**
 * @file
 *
 * @ingroup ClassicTasksImpl
 *
 * @brief Classic Tasks Manager Data Structures
 */

/*
 * COPYRIGHT (c) 1989-2014.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_TASKSDATA_H
#define _RTEMS_RTEMS_TASKSDATA_H

#include <rtems/rtems/tasks.h>
#include <rtems/rtems/asrdata.h>
#include <rtems/rtems/eventdata.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ClassicTasksImpl
 *
 * @{
 */

/**
 *  This is the API specific information required by each thread for
 *  the RTEMS API to function correctly.
 *
 */
typedef struct {
  /** This field contains the event control for this task. */
  Event_Control            Event;
  /** This field contains the system event control for this task. */
  Event_Control            System_event;
  /** This field contains the Classic API Signal information for this task. */
  ASR_Information          Signal;

  /**
   * @brief Signal post-switch action in case signals are pending.
   */
  Thread_Action            Signal_action;
}  RTEMS_API_Control;

/**
 *  @brief _RTEMS_tasks_Initialize_user_tasks_body
 *
 *  This routine creates and starts all configured user
 *  initialization threads.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  RTEMS Task Manager
 */

extern void _RTEMS_tasks_Initialize_user_tasks_body( void );

/**
 *  The following instantiates the information control block used to
 *  manage this class of objects.
 */
extern Thread_Information _RTEMS_tasks_Information;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
