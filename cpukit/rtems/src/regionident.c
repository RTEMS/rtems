/**
 *  @file
 *
 *  @brief RTEMS Region Name to Id
 *  @ingroup ClassicRegion
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/regionimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/apimutex.h>

rtems_status_code rtems_region_ident(
  rtems_name  name,
  rtems_id   *id
)
{
  Objects_Name_or_id_lookup_errors  status;

  status = _Objects_Name_to_id_u32(
    &_Region_Information,
    name,
    OBJECTS_SEARCH_LOCAL_NODE,
    id
  );

  return _Status_Object_name_errors_to_status[ status ];
}
