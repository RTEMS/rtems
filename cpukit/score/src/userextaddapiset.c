/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/userext.h>

/**
 *  This routine is used to add an API extension set to the active list.
 */
void _User_extensions_Add_API_set (
  User_extensions_Control *the_extension
)
{
  _Chain_Append( &_User_extensions_List, &the_extension->Node );

  /*
   *  If a switch handler is present, append it to the switch chain.
   *
   *  NOTE: The Classic API has an extension.  Neither POSIX nor ITRON do.
   *        So if they are not configured, then do not check for NULL.
   */
#if defined(RTEMS_POSIX_API) || defined(RTEMS_ITRON_API)
  if ( the_extension->Callouts.thread_switch != NULL )
#endif
  {
    the_extension->Switch.thread_switch = the_extension->Callouts.thread_switch;
    _Chain_Append(
      &_User_extensions_Switches_list, &the_extension->Switch.Node );
  }
}
