/**
 * @file rtems/rtems/part.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Partition Manager.  This manager provides facilities to
 *  dynamically allocate memory in fixed-sized units which are returned
 *  as buffers.
 *
 *  Directives provided are:
 *
 *     - create a partition
 *     - get an ID of a partition
 *     - delete a partition
 *     - get a buffer from a partition
 *     - return a buffer to a partition
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_PART_H
#define _RTEMS_RTEMS_PART_H

/**
 *  This constant is defined to extern most of the time when using
 *  this header file.  However by defining it to nothing, the data
 *  declared in this header file can be instantiated.  This is done
 *  in a single per manager file.
 */
#ifndef RTEMS_PART_EXTERN
#define RTEMS_PART_EXTERN extern
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/address.h>
#include <rtems/score/object.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/types.h>

/**
 *  @defgroup ClassicPart Partitions
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality related to the
 *  Classic API Partition Manager.
 */
/**@{*/

/**
 *  The following defines the control block used to manage each partition.
 */
typedef struct {
  /** This field is the object management portion of a Partition instance. */
  Objects_Control     Object;
  /** This field is the physical starting address of the Partition. */
  void               *starting_address;
  /** This field is the size of the Partition in bytes. */
  intptr_t            length;
  /** This field is the size of each buffer in bytes */
  uint32_t            buffer_size;
  /** This field is the attribute set provided at create time. */
  rtems_attribute     attribute_set;
  /** This field is the of allocated buffers. */
  uint32_t            number_of_used_blocks;
  /** This field is the chain used to manage unallocated buffers. */
  Chain_Control       Memory;
}   Partition_Control;

/**
 *  The following defines the information control block used to
 *  manage this class of objects.
 */
RTEMS_PART_EXTERN Objects_Information _Partition_Information;

/**
 *  @brief Partition_Manager_initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _Partition_Manager_initialization(void);

/**
 *  @brief rtems_partition_create
 *
 *  This routine implements the rtems_partition_create directive.  The
 *  partition will have the name name.  The memory area managed by
 *  the partition is of length bytes and starts at starting_address.
 *  The memory area will be divided into as many buffers of
 *  buffer_size bytes as possible.   The attribute_set determines if
 *  the partition is global or local.  It returns the id of the
 *  created partition in ID.
 */
rtems_status_code rtems_partition_create(
  rtems_name       name,
  void            *starting_address,
  uint32_t         length,
  uint32_t         buffer_size,
  rtems_attribute  attribute_set,
  rtems_id        *id
);

/**
 *  @brief rtems_partition_ident
 *
 *  This routine implements the rtems_partition_ident directive.
 *  This directive returns the partition ID associated with name.
 *  If more than one partition is named name, then the partition
 *  to which the ID belongs is arbitrary.  node indicates the
 *  extent of the search for the ID of the partition named name.
 *  The search can be limited to a particular node or allowed to
 *  encompass all nodes.
 */
rtems_status_code rtems_partition_ident(
  rtems_name  name,
  uint32_t    node,
  rtems_id   *id
);

/**
 *  @brief rtems_partition_delete
 *
 *  This routine implements the rtems_partition_delete directive.  The
 *  partition indicated by ID is deleted.
 */
rtems_status_code rtems_partition_delete(
  rtems_id id
);

/**
 *  @brief rtems_partition_get_buffer
 *
 *  This routine implements the rtems_partition_get_buffer directive.  It
 *  attempts to allocate a buffer from the partition associated with ID.
 *  If a buffer is allocated, its address is returned in buffer.
 */
rtems_status_code rtems_partition_get_buffer(
  rtems_id   id,
  void     **buffer
);

/**
 *  @brief rtems_partition_return_buffer
 *
 *  This routine implements the rtems_partition_return_buffer directive.  It
 *  frees the buffer to the partition associated with ID.  The buffer must
 *  have been previously allocated from the same partition.
 */
rtems_status_code rtems_partition_return_buffer(
  rtems_id  id,
  void     *buffer
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/part.inl>
#endif
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/partmp.h>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
