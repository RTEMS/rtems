/**
 * @file
 *
 * @ingroup ClassicClassInfo Object Class Information
 *
 * @brief Get API Portion of Object Id
 */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>

/*
 * This is implemented as a macro. This body is provided to support
 * bindings from non-C based languages.
 */
int rtems_object_id_get_api(
  Objects_Id id
);

int rtems_object_id_get_api(
  Objects_Id id
)
{
  return _Objects_Get_API( id );
}
