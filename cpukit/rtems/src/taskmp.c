/*
 *  Multiprocessing Support for the RTEMS Task Manager
 *
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/score/mpci.h>
#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>
#include <rtems/rtems/support.h>

RTEMS_STATIC_ASSERT(
  sizeof(RTEMS_tasks_MP_Packet) <= MP_PACKET_MINIMUM_PACKET_SIZE,
  RTEMS_tasks_MP_Packet
);

/*
 *  _RTEMS_tasks_MP_Send_process_packet
 *
 */

void _RTEMS_tasks_MP_Send_process_packet (
  RTEMS_tasks_MP_Remote_operations operation,
  Objects_Id                       task_id,
  rtems_name                       name
)
{
  RTEMS_tasks_MP_Packet *the_packet;

  switch ( operation ) {

    case RTEMS_TASKS_MP_ANNOUNCE_CREATE:
    case RTEMS_TASKS_MP_ANNOUNCE_DELETE:

      the_packet                    = _RTEMS_tasks_MP_Get_packet();
      the_packet->Prefix.the_class  = MP_PACKET_TASKS;
      the_packet->Prefix.length     = sizeof ( RTEMS_tasks_MP_Packet );
      the_packet->Prefix.to_convert = sizeof ( RTEMS_tasks_MP_Packet );
      the_packet->operation         = operation;
      the_packet->Prefix.id         = task_id;
      the_packet->name              = name;

      _MPCI_Send_process_packet( MPCI_ALL_NODES, &the_packet->Prefix );
      break;

    case RTEMS_TASKS_MP_SUSPEND_REQUEST:
    case RTEMS_TASKS_MP_SUSPEND_RESPONSE:
    case RTEMS_TASKS_MP_RESUME_REQUEST:
    case RTEMS_TASKS_MP_RESUME_RESPONSE:
    case RTEMS_TASKS_MP_SET_PRIORITY_REQUEST:
    case RTEMS_TASKS_MP_SET_PRIORITY_RESPONSE:
    case RTEMS_TASKS_MP_GET_NOTE_REQUEST:
    case RTEMS_TASKS_MP_GET_NOTE_RESPONSE:
    case RTEMS_TASKS_MP_SET_NOTE_REQUEST:
    case RTEMS_TASKS_MP_SET_NOTE_RESPONSE:
      break;
  }
}

/*
 *  _RTEMS_tasks_MP_Send_request_packet
 *
 */

rtems_status_code _RTEMS_tasks_MP_Send_request_packet (
  RTEMS_tasks_MP_Remote_operations operation,
  Objects_Id                       task_id,
  rtems_task_priority                 new_priority,
  uint32_t                         notepad,
  uint32_t                         note
)
{
  RTEMS_tasks_MP_Packet *the_packet;

  switch ( operation ) {

    case RTEMS_TASKS_MP_SUSPEND_REQUEST:
    case RTEMS_TASKS_MP_RESUME_REQUEST:
    case RTEMS_TASKS_MP_SET_PRIORITY_REQUEST:
    case RTEMS_TASKS_MP_GET_NOTE_REQUEST:
    case RTEMS_TASKS_MP_SET_NOTE_REQUEST:

      the_packet                    = _RTEMS_tasks_MP_Get_packet();
      the_packet->Prefix.the_class  = MP_PACKET_TASKS;
      the_packet->Prefix.length     = sizeof ( RTEMS_tasks_MP_Packet );
      the_packet->Prefix.to_convert = sizeof ( RTEMS_tasks_MP_Packet );
      the_packet->operation         = operation;
      the_packet->Prefix.id         = task_id;
      the_packet->the_priority      = new_priority;
      the_packet->notepad           = notepad;
      the_packet->note              = note;

      return _MPCI_Send_request_packet(
        _Objects_Get_node( task_id ),
        &the_packet->Prefix,
        STATES_READY     /* Not used */
      );
      break;

    case RTEMS_TASKS_MP_ANNOUNCE_CREATE:
    case RTEMS_TASKS_MP_ANNOUNCE_DELETE:
    case RTEMS_TASKS_MP_SUSPEND_RESPONSE:
    case RTEMS_TASKS_MP_RESUME_RESPONSE:
    case RTEMS_TASKS_MP_SET_PRIORITY_RESPONSE:
    case RTEMS_TASKS_MP_GET_NOTE_RESPONSE:
    case RTEMS_TASKS_MP_SET_NOTE_RESPONSE:
      break;

  }
  /*
   *  The following line is included to satisfy compilers which
   *  produce warnings when a function does not end with a return.
   */
  return RTEMS_SUCCESSFUL;
}

/*
 *  _RTEMS_tasks_MP_Send_response_packet
 *
 */

void _RTEMS_tasks_MP_Send_response_packet (
  RTEMS_tasks_MP_Remote_operations  operation,
  Thread_Control                   *the_thread
)
{
  RTEMS_tasks_MP_Packet *the_packet;

  switch ( operation ) {

    case RTEMS_TASKS_MP_SUSPEND_RESPONSE:
    case RTEMS_TASKS_MP_RESUME_RESPONSE:
    case RTEMS_TASKS_MP_SET_PRIORITY_RESPONSE:
    case RTEMS_TASKS_MP_GET_NOTE_RESPONSE:
    case RTEMS_TASKS_MP_SET_NOTE_RESPONSE:

      the_packet = (RTEMS_tasks_MP_Packet *) the_thread->receive_packet;

/*
 *  The packet being returned already contains the class, length, and
 *  to_convert fields, therefore they are not set in this routine.
 */
      the_packet->operation    = operation;
      the_packet->Prefix.id    = the_packet->Prefix.source_tid;

      _MPCI_Send_response_packet(
        _Objects_Get_node( the_packet->Prefix.source_tid ),
        &the_packet->Prefix
      );
      break;

    case RTEMS_TASKS_MP_ANNOUNCE_CREATE:
    case RTEMS_TASKS_MP_ANNOUNCE_DELETE:
    case RTEMS_TASKS_MP_SUSPEND_REQUEST:
    case RTEMS_TASKS_MP_RESUME_REQUEST:
    case RTEMS_TASKS_MP_SET_PRIORITY_REQUEST:
    case RTEMS_TASKS_MP_GET_NOTE_REQUEST:
    case RTEMS_TASKS_MP_SET_NOTE_REQUEST:
      break;

  }
}

/*
 *
 *  _RTEMS_tasks_MP_Process_packet
 *
 */

void _RTEMS_tasks_MP_Process_packet (
  rtems_packet_prefix  *the_packet_prefix
)
{
  RTEMS_tasks_MP_Packet *the_packet;
  Thread_Control   *the_thread;
  bool           ignored;

  the_packet = (RTEMS_tasks_MP_Packet *) the_packet_prefix;

  switch ( the_packet->operation ) {

    case RTEMS_TASKS_MP_ANNOUNCE_CREATE:

      ignored = _Objects_MP_Allocate_and_open(
                  &_RTEMS_tasks_Information,
                  the_packet->name,
                  the_packet->Prefix.id,
                  true
                );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case RTEMS_TASKS_MP_ANNOUNCE_DELETE:

      _Objects_MP_Close( &_RTEMS_tasks_Information, the_packet->Prefix.id );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case RTEMS_TASKS_MP_SUSPEND_REQUEST:

      the_packet->Prefix.return_code = rtems_task_suspend(
        the_packet->Prefix.id
      );

      _RTEMS_tasks_MP_Send_response_packet(
        RTEMS_TASKS_MP_SUSPEND_RESPONSE,
        _Thread_Executing
      );
      break;

    case RTEMS_TASKS_MP_SUSPEND_RESPONSE:
    case RTEMS_TASKS_MP_RESUME_RESPONSE:
    case RTEMS_TASKS_MP_SET_NOTE_RESPONSE:

      the_thread = _MPCI_Process_response( the_packet_prefix );

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case RTEMS_TASKS_MP_RESUME_REQUEST:

      the_packet->Prefix.return_code = rtems_task_resume(
        the_packet->Prefix.id
      );

      _RTEMS_tasks_MP_Send_response_packet(
        RTEMS_TASKS_MP_RESUME_RESPONSE,
        _Thread_Executing
      );
      break;

    case RTEMS_TASKS_MP_SET_PRIORITY_REQUEST:

      the_packet->Prefix.return_code = rtems_task_set_priority(
        the_packet->Prefix.id,
        the_packet->the_priority,
        &the_packet->the_priority
      );

      _RTEMS_tasks_MP_Send_response_packet(
        RTEMS_TASKS_MP_SET_PRIORITY_RESPONSE,
        _Thread_Executing
      );
      break;

    case RTEMS_TASKS_MP_SET_PRIORITY_RESPONSE:

      the_thread = _MPCI_Process_response( the_packet_prefix );

      *(rtems_task_priority *)the_thread->Wait.return_argument =
                                               the_packet->the_priority;

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case RTEMS_TASKS_MP_GET_NOTE_REQUEST:

      the_packet->Prefix.return_code = rtems_task_get_note(
        the_packet->Prefix.id,
        the_packet->notepad,
        &the_packet->note
      );

      _RTEMS_tasks_MP_Send_response_packet(
        RTEMS_TASKS_MP_GET_NOTE_RESPONSE,
        _Thread_Executing
      );
      break;

    case RTEMS_TASKS_MP_GET_NOTE_RESPONSE:

      the_thread = _MPCI_Process_response( the_packet_prefix );

      *(uint32_t   *)the_thread->Wait.return_argument = the_packet->note;

      _MPCI_Return_packet( the_packet_prefix );
      break;

    case RTEMS_TASKS_MP_SET_NOTE_REQUEST:

      the_packet->Prefix.return_code = rtems_task_set_note(
        the_packet->Prefix.id,
        the_packet->notepad,
        the_packet->note
      );

      _RTEMS_tasks_MP_Send_response_packet(
        RTEMS_TASKS_MP_SET_NOTE_RESPONSE,
        _Thread_Executing
      );
      break;
  }
}

/*
 *  _RTEMS_tasks_MP_Send_object_was_deleted
 *
 *  This routine is not neededby the Tasks since a task
 *  cannot be globally deleted.
 *
 */

/*
 *  _RTEMS_tasks_MP_Send_extract_proxy
 *
 *  This routine is not neededby the Tasks since a task
 *  cannot be globally deleted.
 *
 */

/*
 *  _RTEMS_tasks_MP_Get_packet
 *
 */

RTEMS_tasks_MP_Packet *_RTEMS_tasks_MP_Get_packet ( void )
{
  return (RTEMS_tasks_MP_Packet *) _MPCI_Get_packet();
}

/* end of file */
