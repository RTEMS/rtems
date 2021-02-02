/**
 * @file
 *
 * @ingroup RTEMSImplClassicObject
 *
 * @brief This source file contains the implementation of
 *   rtems_build_name() as a binding for languages other than C/C++.
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

static rtems_name _RTEMS_Build_name( char c1, char c2, char c3, char c4 )
{
  return rtems_build_name( c1, c2, c3, c4 );
}

#undef rtems_build_name

rtems_id rtems_build_name( char c1, char c2, char c3, char c4 )
{
  return _RTEMS_Build_name( c1, c2, c3, c4 );
}
