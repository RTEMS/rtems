/**
 * @file
 *
 * @ingroup ClassicTaskMP
 *
 * This include file contains all the constants and structures associated
 * with the multiprocessing support in the task manager.
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_TASKMP_H
#define _RTEMS_RTEMS_TASKMP_H

#ifndef _RTEMS_RTEMS_TASKSIMPL_H
# error "Never use <rtems/rtems/taskmp.h> directly; include <rtems/rtems/tasksimpl.h> instead."
#endif

#include <rtems/score/mpciimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicTaskMP Task MP Support
 *
 *  @ingroup ClassicMP
 *
 *  This encapsulates functionality related to the transparent multiprocessing
 *  support within the Classic API Task Manager.
 */
/**@{*/

/**
 *  The following enumerated type defines the list of
 *  remote task operations.
 */
typedef enum {
  RTEMS_TASKS_MP_ANNOUNCE_CREATE       =  0,
  RTEMS_TASKS_MP_ANNOUNCE_DELETE       =  1,
  RTEMS_TASKS_MP_SUSPEND_REQUEST       =  2,
  RTEMS_TASKS_MP_SUSPEND_RESPONSE      =  3,
  RTEMS_TASKS_MP_RESUME_REQUEST        =  4,
  RTEMS_TASKS_MP_RESUME_RESPONSE       =  5,
  RTEMS_TASKS_MP_SET_PRIORITY_REQUEST  =  6,
  RTEMS_TASKS_MP_SET_PRIORITY_RESPONSE =  7,
}   RTEMS_tasks_MP_Remote_operations;

/**
 *  @brief RTEMS Tasks MP Send Process Packet
 *
 *  Multiprocessing Support for the RTEMS Task Manager
 *
 *  This routine performs a remote procedure call so that a
 *  process operation can be performed on another node.
 */
void _RTEMS_tasks_MP_Send_process_packet (
  RTEMS_tasks_MP_Remote_operations operation,
  Objects_Id                       task_id,
  rtems_name                       name
);

/**
 * @brief Issues a remote rtems_task_set_priority() request.
 */
rtems_status_code _RTEMS_tasks_MP_Set_priority(
  rtems_id             id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority
);

/**
 * @brief Issues a remote rtems_task_suspend() request.
 */
rtems_status_code _RTEMS_tasks_MP_Suspend( rtems_id id );

/**
 * @brief Issues a remote rtems_task_resume() request.
 */
rtems_status_code _RTEMS_tasks_MP_Resume( rtems_id id );

/**
 *  @brief _RTEMS_tasks_MP_Process_packet
 *
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */
void _RTEMS_tasks_MP_Process_packet (
  rtems_packet_prefix *the_packet_prefix
);

/**
 *  @brief _RTEMS_tasks_MP_Send_object_was_deleted
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed by RTEMS_tasks since a task
 *  cannot be deleted when segments are in use.
 */

/*
 *  _RTEMS_tasks_MP_Send_extract_proxy
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed since there are no objects
 *  deleted by this manager.
 *
 */

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of file */
