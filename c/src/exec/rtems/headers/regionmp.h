/*  regionmp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Multiprocessing Support in the Region Manager.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_REGION_MP_h
#define __RTEMS_REGION_MP_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/core/mppkt.h>
#include <rtems/core/object.h>
#include <rtems/core/thread.h>

#include <rtems/rtems/options.h>
#include <rtems/rtems/region.h>

/*
 *  The following enumerated type defines the list of
 *  remote region operations.
 */

typedef enum {
  REGION_MP_ANNOUNCE_CREATE         =  0,
  REGION_MP_ANNOUNCE_DELETE         =  1,
  REGION_MP_EXTRACT_PROXY           =  2,
  REGION_MP_GET_SEGMENT_REQUEST     =  3,
  REGION_MP_GET_SEGMENT_RESPONSE    =  4,
  REGION_MP_RETURN_SEGMENT_REQUEST  =  5,
  REGION_MP_RETURN_SEGMENT_RESPONSE =  6
}   Region_MP_Remote_operations;

/*
 *  The following data structure defines the packet used to perform
 *  remote region operations.
 */

typedef struct {
  rtems_packet_prefix          Prefix;
  Region_MP_Remote_operations  operation;
  rtems_name                   name;
  rtems_option                 option_set;
  unsigned32                   size;
  Objects_Id                   proxy_id;
  void                        *segment;
}   Region_MP_Packet;

/*
 *  _Region_MP_Send_process_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  process operation can be performed on another node.
 */

void _Region_MP_Send_process_packet (
  Region_MP_Remote_operations operation,
  Objects_Id                  region_id,
  rtems_name                  name,
  Objects_Id                  proxy_id
);

/*
 *  _Region_MP_Send_request_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  directive operation can be initiated on another node.
 */

rtems_status_code _Region_MP_Send_request_packet (
  Region_MP_Remote_operations  operation,
  Objects_Id                   region_id,
  void                        *segment,
  unsigned32                   size,
  rtems_option                 option_set,
  rtems_interval               timeout
);

/*
 *  _Region_MP_Send_response_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  directive can be performed on another node.
 */

void _Region_MP_Send_response_packet (
  Region_MP_Remote_operations  operation,
  Objects_Id                   region_id,
  Thread_Control              *the_thread
);

/*
 *
 *  _Region_MP_Process_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */

void _Region_MP_Process_packet (
  rtems_packet_prefix *the_packet_prefix
);

/*
 *  _Region_MP_Send_object_was_deleted
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed by the Region since a region
 *  cannot be deleted when segments are in use.
 */

/*
 *  _Region_MP_Send_extract_proxy
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 */

void _Region_MP_Send_extract_proxy (
  Thread_Control *the_thread
);

/*
 *  _Region_MP_Get_packet
 *
 *  DESCRIPTION:
 *
 *  This function is used to obtain a region mp packet.
 */

Region_MP_Packet *_Region_MP_Get_packet ( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of file */
