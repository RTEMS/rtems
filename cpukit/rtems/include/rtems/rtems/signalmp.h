/*  signalmp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Multiprocessing Support in the Signal Manager.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_SIGNAL_MP_h
#define __RTEMS_SIGNAL_MP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/asr.h>
#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>

/*
 *  The following enumerated type defines the list of
 *  remote signal operations.
 */

typedef enum {
  SIGNAL_MP_SEND_REQUEST  = 0,
  SIGNAL_MP_SEND_RESPONSE = 1
}   Signal_MP_Remote_operations;

/*
 *  The following data structure defines the packet used to perform
 *  remote signal operations.
 */

typedef struct {
  rtems_packet_prefix          Prefix;
  Signal_MP_Remote_operations  operation;
  rtems_signal_set             signal_in;
}   Signal_MP_Packet;

/*
 *  _Signal_MP_Send_process_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  process operation can be performed on another node.
 *
 *  This routine is not needed since there are no process
 *  packets to be sent by this manager.
 */

/*
 *  _Signal_MP_Send_request_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  directive operation can be initiated on another node.
 */

rtems_status_code _Signal_MP_Send_request_packet (
  Signal_MP_Remote_operations operation,
  Objects_Id                  task_id,
  rtems_signal_set      signal_in
);

/*
 *  _Signal_MP_Send_response_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  directive can be performed on another node.
 */

void _Signal_MP_Send_response_packet (
  Signal_MP_Remote_operations  operation,
  Thread_Control              *the_thread
);

/*
 *
 *  _Signal_MP_Process_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */

void _Signal_MP_Process_packet (
  rtems_packet_prefix *the_packet_prefix
);

/*
 *  _Signal_MP_Send_object_was_deleted
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed since there are no objects
 *  deleted by this manager.
 */

/*
 *  _Signal_MP_Send_extract_proxy
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed since there are no objects
 *  deleted by this manager.
 */

/*
 *  _Signal_MP_Get_packet
 *
 *  DESCRIPTION:
 *
 *  This function is used to obtain a signal mp packet.
 */

Signal_MP_Packet *_Signal_MP_Get_packet ( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of file */
