/**
 * @file
 *
 * @brief MP Support in Partition Manager
 *
 * This include file contains all the constants and structures associated
 * with the Multiprocessing Support in the Partition Manager.
 */

/* COPYRIGHT (c) 1989-2013.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_PARTMP_H
#define _RTEMS_RTEMS_PARTMP_H

#ifndef _RTEMS_RTEMS_PARTIMPL_H
# error "Never use <rtems/rtems/partmp.h> directly; include <rtems/rtems/partimpl.h> instead."
#endif

#include <rtems/score/mpciimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicPartMP Partition MP Support
 *
 *  @ingroup ClassicMP
 *
 *  This encapsulates functionality related to the transparent multiprocessing
 *  support within the Classic API Partition Manager.
 */
/*{*/

/**
 *  The following enumerated type defines the list of
 *  remote partition operations.
 */
typedef enum {
  PARTITION_MP_ANNOUNCE_CREATE        =  0,
  PARTITION_MP_ANNOUNCE_DELETE        =  1,
  PARTITION_MP_EXTRACT_PROXY          =  2,
  PARTITION_MP_GET_BUFFER_REQUEST     =  3,
  PARTITION_MP_GET_BUFFER_RESPONSE    =  4,
  PARTITION_MP_RETURN_BUFFER_REQUEST  =  5,
  PARTITION_MP_RETURN_BUFFER_RESPONSE =  6
}   Partition_MP_Remote_operations;

/**
 *  The following data structure defines the packet used to perform
 *  remote partition operations.
 */
typedef struct {
  rtems_packet_prefix             Prefix;
  Partition_MP_Remote_operations  operation;
  rtems_name                      name;
  void                           *buffer;
  Objects_Id                      proxy_id;
}   Partition_MP_Packet;

RTEMS_INLINE_ROUTINE bool _Partition_MP_Is_remote( Objects_Id id )
{
  return _Objects_MP_Is_remote( id, &_Partition_Information );
}

/**
 *  @brief Partition_MP_Send_process_packet
 *
 *  Multiprocessing Support for the Partition Manager
 *
 *  This routine performs a remote procedure call so that a
 *  process operation can be performed on another node.
 */
void _Partition_MP_Send_process_packet (
  Partition_MP_Remote_operations operation,
  Objects_Id                     partition_id,
  rtems_name                     name,
  Objects_Id                     proxy_id
);

/**
 * @brief Issues a remote rtems_partition_get_buffer() request.
 */
rtems_status_code _Partition_MP_Get_buffer(
  rtems_id   id,
  void     **buffer
);

/**
 * @brief Issues a remote rtems_partition_return_buffer() request.
 */
rtems_status_code _Partition_MP_Return_buffer(
  rtems_id  id,
  void     *buffer
);

/*
 *  @brief Partition_MP_Send_object_was_deleted
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 *
 *  This routine is not needed by the Partition since a partition
 *  cannot be deleted when buffers are in use.
 */

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of file */
