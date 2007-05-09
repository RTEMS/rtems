/*
 *  COPYRIGHT (c) 1989-2007.
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
 *  This routine is used to add a user extension set to the active list.
 *
 *  @note Must be before _User_extensions_Handler_initialization to
 *        ensure proper inlining.
 */

void _User_extensions_Add_set (
  User_extensions_Control *the_extension,
  User_extensions_Table   *extension_table
)
{
  the_extension->Callouts = *extension_table;

  _Chain_Append( &_User_extensions_List, &the_extension->Node );

  /*
   * If a switch handler is present, append it to the switch chain.
   */

  if ( extension_table->thread_switch != NULL ) {
    the_extension->Switch.thread_switch = extension_table->thread_switch;
    _Chain_Append(
      &_User_extensions_Switches_list,
      &the_extension->Switch.Node
    );
  }
}
