/*  partition.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Partition Manager.  This manager provides facilities to
 *  dynamically allocate memory in fixed-sized units which are returned
 *  as buffers.
 *
 *  Directives provided are:
 *
 *     + create a partition
 *     + get an ID of a partition
 *     + delete a partition
 *     + get a buffer from a partition
 *     + return a buffer to a partition
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

#ifndef __RTEMS_PARTITION_h
#define __RTEMS_PARTITION_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/address.h>
#include <rtems/score/object.h>
#include <rtems/rtems/attr.h>
#include <rtems/rtems/types.h>

/*
 *  The following defines the control block used to manage each partition.
 */

typedef struct {
  Objects_Control     Object;
  void               *starting_address;      /* physical address */
  unsigned32          length;                /* in bytes */
  unsigned32          buffer_size;           /* in bytes */
  rtems_attribute  attribute_set;         /* attributes */
  unsigned32          number_of_used_blocks; /* or allocated buffers */
  Chain_Control       Memory;                /* buffer chain */
}   Partition_Control;

/*
 *  The following defines the information control block used to
 *  manage this class of objects.
 */

RTEMS_EXTERN Objects_Information _Partition_Information;

/*
 *  _Partition_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _Partition_Manager_initialization(
  unsigned32 maximum_partitions
);

/*
 *  rtems_partition_create
 *
 *  DESCRIPTION:
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
  rtems_name          name,
  void               *starting_address,
  unsigned32          length,
  unsigned32          buffer_size,
  rtems_attribute  attribute_set,
  Objects_Id         *id
);

/*
 *  rtems_partition_ident
 *
 *  DESCRIPTION:
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
  rtems_name    name,
  unsigned32    node,
  Objects_Id   *id
);

/*
 *  rtems_partition_delete
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_partition_delete directive.  The
 *  partition indicated by ID is deleted.
 */

rtems_status_code rtems_partition_delete(
  Objects_Id id
);

/*
 *  rtems_partition_get_buffer
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_partition_get_buffer directive.  It
 *  attempts to allocate a buffer from the partition associated with ID.
 *  If a buffer is allocated, its address is returned in buffer.
 */

rtems_status_code rtems_partition_get_buffer(
  Objects_Id  id,
  void       **buffer
);

/*
 *  rtems_partition_return_buffer
 *
 *  DESCRIPTION:
 *
 *  This routine implements the rtems_partition_return_buffer directive.  It
 *  frees the buffer to the partition associated with ID.  The buffer must
 *  have been previously allocated from the same partition.
 */

rtems_status_code rtems_partition_return_buffer(
  Objects_Id  id,
  void       *buffer
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

#endif
/* end of include file */
