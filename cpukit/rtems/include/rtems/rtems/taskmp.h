/**
 * @file rtems/rtems/taskmp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the multiprocessing support in the task manager.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_TASKMP_H
#define _RTEMS_RTEMS_TASKMP_H

#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/score/priority.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/thread.h>

/**
 *  @defgroup ClassicTaskMP Task MP Support
 *
 *  @ingroup ClassicMP
 *
 *  This encapsulates functionality which XXX
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif
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
  RTEMS_TASKS_MP_GET_NOTE_REQUEST      =  8,
  RTEMS_TASKS_MP_GET_NOTE_RESPONSE     =  9,
  RTEMS_TASKS_MP_SET_NOTE_REQUEST      = 10,
  RTEMS_TASKS_MP_SET_NOTE_RESPONSE     = 11
}   RTEMS_tasks_MP_Remote_operations;

/**
 *  The following data structure defines the packet used to perform
 *  remote task operations.
 */
typedef struct {
  rtems_packet_prefix               Prefix;
  RTEMS_tasks_MP_Remote_operations  operation;
  rtems_name                        name;
  rtems_task_priority               the_priority;
  uint32_t                          notepad;
  uint32_t                          note;
}   RTEMS_tasks_MP_Packet;

/**
 *  @brief _RTEMS_tasks_MP_Send_process_packet
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
 *  @brief _RTEMS_tasks_MP_Send_request_packet
 *
 *  This routine performs a remote procedure call so that a
 *  directive operation can be initiated on another node.
 */
rtems_status_code _RTEMS_tasks_MP_Send_request_packet (
  RTEMS_tasks_MP_Remote_operations operation,
  Objects_Id                       task_id,
  rtems_task_priority                 the_priority,
  uint32_t                         notepad,
  uint32_t                         note
);

/**
 *  @brief _RTEMS_tasks_MP_Send_response_packet
 *
 *  This routine performs a remote procedure call so that a
 *  directive can be performed on another node.
 */
void _RTEMS_tasks_MP_Send_response_packet (
  RTEMS_tasks_MP_Remote_operations  operation,
  Thread_Control                   *the_thread
);

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

/**
 *  @brief _RTEMS_tasks_MP_Get_packet
 *
 *  This function is used to obtain a task mp packet.
 */
RTEMS_tasks_MP_Packet *_RTEMS_tasks_MP_Get_packet ( void );

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of file */
