/**
 *  @file
 *
 *  @brief Get Lowest Valid API Index
 *  @ingroup ClassicClassInfo
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
int rtems_object_id_api_minimum(void);

int rtems_object_id_api_minimum(void)
{
  return OBJECTS_INTERNAL_API;
}
