/*
 *  Debug Manager
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
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
