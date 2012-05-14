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

void _User_extensions_Thread_switch (
  Thread_Control *executing,
  Thread_Control *heir
)
{
  Chain_Node                     *the_node;
  User_extensions_Switch_control *the_extension_switch;

  for ( the_node = _Chain_First( &_User_extensions_Switches_list );
        !_Chain_Is_tail( &_User_extensions_Switches_list, the_node ) ;
        the_node = the_node->next ) {

    the_extension_switch = (User_extensions_Switch_control *) the_node;

    (*the_extension_switch->thread_switch)( executing, heir );
  }
}
