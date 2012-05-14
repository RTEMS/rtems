/*
 *  freenode()
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications to support reference counting in the file system are
 *  Copyright (c) 2012 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/libio_.h>

void rtems_filesystem_location_free( rtems_filesystem_location_info_t *loc )
{
  rtems_filesystem_instance_lock( loc );
  (*loc->mt_entry->ops->freenod_h)( loc );
  rtems_filesystem_instance_unlock( loc );
  rtems_filesystem_location_remove_from_mt_entry( loc );
}
