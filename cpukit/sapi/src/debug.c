/*
 *  Debug Manager
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/debug.h>

/*PAGE
 *
 *  _Debug_Manager_initialization
 */
 
void _Debug_Manager_initialization( void )
{
  rtems_debug_disable( RTEMS_DEBUG_ALL_MASK );
}
 
/*PAGE
 *
 *  rtems_debug_enable
 */
 
void rtems_debug_enable (
  rtems_debug_control  to_be_enabled
)
{
  _Debug_Level |= to_be_enabled;
}
 
/*PAGE
 *
 *  rtems_debug_disable
 */
 
void rtems_debug_disable (
  rtems_debug_control  to_be_disabled
)
{
  _Debug_Level &= ~to_be_disabled;
}

/*PAGE
 *
 *  _Debug_Is_enabled
 */

boolean _Debug_Is_enabled(
  rtems_debug_control  level
)
{
  return (_Debug_Level & level);
}
