/*
 *  Extension Manager
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
#include <rtems/object.h>
#include <rtems/thread.h>
#include <rtems/extension.h>

void _Extension_Manager_initialization(
  unsigned32 maximum_extensions
)
{
}

rtems_status_code rtems_extension_create(
  rtems_name                     name,
  rtems_extensions_table *extension_table,
  Objects_Id                    *id
)
{
  return( RTEMS_NOT_CONFIGURED );
}

rtems_status_code rtems_extension_ident(
  rtems_name    name,
  Objects_Id   *id
)
{
  return( RTEMS_NOT_CONFIGURED );
}

rtems_status_code rtems_extension_delete(
  Objects_Id id
)
{
  return( RTEMS_NOT_CONFIGURED );
}
