/**
 *  @file
 *
 *  @brief Build Thirty-Two Bit Object Name
 *  @ingroup ClassicClassInfo
 */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/types.h>

/*
 *  Undefine since this is normally a macro and we want a real body in
 *  the library for other language bindings.
 */
#undef rtems_build_name

/*
 *  Prototype it to avoid warnings
 */
rtems_name rtems_build_name(
  char C1,
  char C2,
  char C3,
  char C4
);

/*
 *  Now define a real body
 */
rtems_name rtems_build_name(
  char C1,
  char C2,
  char C3,
  char C4
)
{
  return _Objects_Build_name( C1, C2, C3, C4 );
}
