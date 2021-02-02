/**
 * @file
 *
 * @ingroup RTEMSImplClassicObject
 *
 * @brief This source file contains the implementation of
 *   rtems_object_id_api_maximum() as a binding for languages other than C/C++.
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

static int _RTEMS_Object_id_api_maximum( void )
{
  return rtems_object_id_api_maximum();
}

#undef rtems_object_id_api_maximum

int rtems_object_id_api_maximum( void )
{
  return _RTEMS_Object_id_api_maximum();
}
