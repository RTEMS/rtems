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

#ifndef __RTEMS_PARTITION_h
#define __RTEMS_PARTITION_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <rtems/address.h>
#include <rtems/attr.h>
#include <rtems/object.h>

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

EXTERN Objects_Information _Partition_Information;

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

/*
 *  _Partition_Allocate_buffer
 *
 *  DESCRIPTION:
 *
 *  This function attempts to allocate a buffer from the_partition.
 *  If successful, it returns the address of the allocated buffer.
 *  Otherwise, it returns NULL.
 */

STATIC INLINE void *_Partition_Allocate_buffer (
   Partition_Control *the_partition
);

/*
 *  _Partition_Free_buffer
 *
 *  DESCRIPTION:
 *
 *  This routine frees the_buffer to the_partition.
 */

STATIC INLINE void _Partition_Free_buffer (
  Partition_Control *the_partition,
  Chain_Node        *the_buffer
);

/*
 *  _Partition_Is_buffer_on_boundary
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_buffer is on a valid buffer
 *  boundary for the_partition, and FALSE otherwise.
 */

STATIC INLINE boolean _Partition_Is_buffer_on_boundary (
  void              *the_buffer,
  Partition_Control *the_partition
);

/*
 *  _Partition_Is_buffer_valid
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_buffer is a valid buffer from
 *  the_partition, otherwise FALSE is returned.
 */

STATIC INLINE boolean _Partition_Is_buffer_valid (
  Chain_Node        *the_buffer,
  Partition_Control *the_partition
);

/*
 *  _Partition_Is_buffer_size_aligned
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the use of the specified buffer_size
 *  will result in the allocation of buffers whose first byte is
 *  properly aligned, and FALSE otherwise.
 */

STATIC INLINE boolean _Partition_Is_buffer_size_aligned (
  unsigned32 buffer_size
);

/*
 *  _Partition_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a partition control block from
 *  the inactive chain of free partition control blocks.
 */

STATIC INLINE Partition_Control *_Partition_Allocate ( void );

/*
 *  _Partition_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a partition control block to the
 *  inactive chain of free partition control blocks.
 */

STATIC INLINE void _Partition_Free (
  Partition_Control *the_partition
);

/*
 *  _Partition_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps partition IDs to partition control blocks.
 *  If ID corresponds to a local partition, then it returns
 *  the_partition control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  If the partition ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_partition is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_partition is undefined.
 */

STATIC INLINE Partition_Control *_Partition_Get (
  Objects_Id         id,
  Objects_Locations *location
);

/*
 *  _Partition_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_partition is NULL
 *  and FALSE otherwise.
 */

STATIC INLINE boolean _Partition_Is_null (
  Partition_Control *the_partition
);

#include <rtems/part.inl>
#include <rtems/partmp.h>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
