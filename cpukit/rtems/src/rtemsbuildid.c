/**
 * @file
 *
 * @ingroup RTEMSImplClassicObject
 *
 * @brief This source file contains the implementation of
 *   rtems_build_id() as a binding for languages other than C/C++.
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

#include <rtems/rtems/object.h>

static rtems_id _RTEMS_Build_id(
  uint32_t api,
  uint32_t class,
  uint32_t node,
  uint32_t index
)
{
  return rtems_build_id( api, class, node, index );
}

#undef rtems_build_id

rtems_id rtems_build_id(
  uint32_t api,
  uint32_t class,
  uint32_t node,
  uint32_t index
)
{
  return _RTEMS_Build_id( api, class, node, index );
}
