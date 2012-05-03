/**
 * @file rtems/rtems/regionmp.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Multiprocessing Support in the Region Manager.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_REGIONMP_H
#define _RTEMS_RTEMS_REGIONMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/mppkt.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>

#include <rtems/rtems/options.h>
#include <rtems/rtems/region.h>

/**
 *  @defgroup ClassicRegionMP Region MP Support
 *
 *  @ingroup ClassicMP
 *
 *  This encapsulates functionality which XXX
 */
/**@{*/

/**
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

/**
 *  The following data structure defines the packet used to perform
 *  remote region operations.
 */
typedef struct {
  rtems_packet_prefix          Prefix;
  Region_MP_Remote_operations  operation;
  rtems_name                   name;
  rtems_option                 option_set;
  uint32_t                     size;
  Objects_Id                   proxy_id;
  void                        *segment;
}   Region_MP_Packet;

/**
 *  @brief _Region_MP_Send_process_packet
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

/**
 *  @brief _Region_MP_Send_request_packet
 *
 *  This routine performs a remote procedure call so that a
 *  directive operation can be initiated on another node.
 */
rtems_status_code _Region_MP_Send_request_packet (
  Region_MP_Remote_operations  operation,
  Objects_Id                   region_id,
  void                        *segment,
  intptr_t                     size,
  rtems_option                 option_set,
  rtems_interval               timeout
);

/**
 *  @brief _Region_MP_Send_response_packet
 *
 *  This routine performs a remote procedure call so that a
 *  directive can be performed on another node.
 */
void _Region_MP_Send_response_packet (
  Region_MP_Remote_operations  operation,
  Objects_Id                   region_id,
  Thread_Control              *the_thread
);

/**
 *  @brief _Region_MP_Process_packet
 *
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */
void _Region_MP_Process_packet (
  rtems_packet_prefix *the_packet_prefix
);

/*
 *  @brief _Region_MP_Send_object_was_deleted
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed by the Region since a region
 *  cannot be deleted when segments are in use.
 */

/**
 *  @brief _Region_MP_Send_extract_proxy
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 */
void _Region_MP_Send_extract_proxy (
  void           *argument
);

/**
 *  @brief _Region_MP_Get_packet
 *
 *  This function is used to obtain a region mp packet.
 */
Region_MP_Packet *_Region_MP_Get_packet ( void );

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of file */
