/**
 * @file
 *
 * @ingroup ScoreUserExt
 *
 * @brief User Extension Handler implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2007.
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
#include <rtems/score/userext.h>

void _User_extensions_Add_set(
  User_extensions_Control *the_extension
)
{
  _Chain_Append( &_User_extensions_List, &the_extension->Node );

  /*
   * If a switch handler is present, append it to the switch chain.
   */

  if ( the_extension->Callouts.thread_switch != NULL ) {
    the_extension->Switch.thread_switch =
      the_extension->Callouts.thread_switch;
    _Chain_Append(
      &_User_extensions_Switches_list,
      &the_extension->Switch.Node
    );
  }
}
