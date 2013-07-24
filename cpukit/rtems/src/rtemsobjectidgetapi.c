/**
 * @file
 *
 * @brief Get API Portion of Object Id
 * @ingroup ClassicClassInfo Object Class Information
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>

/* XXX: This is for language bindings */
int rtems_object_id_get_api(
  Objects_Id id
);

int rtems_object_id_get_api(
  Objects_Id id
)
{
  return _Objects_Get_API( id );
}
