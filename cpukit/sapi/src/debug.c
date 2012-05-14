/*
 *  Debug Manager
 *
 *  COPYRIGHT (c) 1989-2009.
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
#include <rtems/debug.h>

/*
 *
 *  _Debug_Manager_initialization
 */
void _Debug_Manager_initialization( void )
{
  rtems_debug_disable( RTEMS_DEBUG_ALL_MASK );
}

/*
 *  rtems_debug_enable
 */
void rtems_debug_enable (
  rtems_debug_control  to_be_enabled
)
{
  _Debug_Level |= to_be_enabled;
}

/*
 *  rtems_debug_disable
 */
void rtems_debug_disable (
  rtems_debug_control  to_be_disabled
)
{
  _Debug_Level &= ~to_be_disabled;
}

/*
 *  rtems_debug_is_enabled
 */
bool rtems_debug_is_enabled(
  rtems_debug_control  level
)
{
  return (_Debug_Level & level) ? true : false;
}
