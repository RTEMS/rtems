/*
 *  Partition Manager
 *
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

#include <rtems/system.h>
#include <rtems/address.h>
#include <rtems/config.h>
#include <rtems/object.h>
#include <rtems/part.h>
#include <rtems/thread.h>

void _Partition_Manager_initialization(
  unsigned32 maximum_partitions
)
{
}

rtems_status_code rtems_partition_create(
  Objects_Name        name,
  void               *starting_address,
  unsigned32          length,
  unsigned32          buffer_size,
  rtems_attribute  attribute_set,
  Objects_Id         *id
)
{
  return( RTEMS_NOT_CONFIGURED );
}

rtems_status_code rtems_partition_ident(
  Objects_Name  name,
  unsigned32    node,
  Objects_Id   *id
)
{
  return( RTEMS_NOT_CONFIGURED );
}

rtems_status_code rtems_partition_delete(
  Objects_Id id
)
{
  return( RTEMS_NOT_CONFIGURED );
}

rtems_status_code rtems_partition_get_buffer(
  Objects_Id   id,
  void       **buffer
)
{
  return( RTEMS_NOT_CONFIGURED );
}

rtems_status_code rtems_partition_return_buffer(
  Objects_Id  id,
  void       *buffer
)
{
  return( RTEMS_NOT_CONFIGURED );
}
