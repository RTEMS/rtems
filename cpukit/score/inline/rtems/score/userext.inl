/** 
 *  @file  rtems/score/userext.inl
 *
 *  This file contains the macro implementation of the inlined routines
 *  from the User Extension Handler
 */

/*
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_USEREXT_INL
#define _RTEMS_SCORE_USEREXT_INL

#include <rtems/score/wkspace.h>

#include <string.h> /* memset */

/**
 *  @addtogroup ScoreUserExt 
 *  @{
 */

/**
 *  This routine is used to add a user extension set to the active list.
 *
 *  @note Must be before _User_extensions_Handler_initialization to
 *        ensure proper inlining.
 */

RTEMS_INLINE_ROUTINE void _User_extensions_Add_set (
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
    _Chain_Append( &_User_extensions_Switches_list, &the_extension->Switch.Node );
  }
}

/**
 *  This routine performs the initialization necessary for this handler.
 */

RTEMS_INLINE_ROUTINE void _User_extensions_Handler_initialization (
    uint32_t                number_of_extensions,
    User_extensions_Table  *initial_extensions
)
{
  User_extensions_Control *extension;
  uint32_t                 i;

  _Chain_Initialize_empty( &_User_extensions_List );
  _Chain_Initialize_empty( &_User_extensions_Switches_list );

  if ( initial_extensions ) {
    extension = (User_extensions_Control *)
      _Workspace_Allocate_or_fatal_error(
        number_of_extensions * sizeof( User_extensions_Control )
      );
  
    memset (
      extension,
      0,
      number_of_extensions * sizeof( User_extensions_Control )
    );
  
    for ( i = 0 ; i < number_of_extensions ; i++ ) {
      _User_extensions_Add_set (extension, &initial_extensions[i]);
      extension++;
    }
  }
}

/**
 *  This routine is used to add an API extension set to the active list.
 */
 
RTEMS_INLINE_ROUTINE void _User_extensions_Add_API_set (
  User_extensions_Control *the_extension
)
{
  _Chain_Append( &_User_extensions_List, &the_extension->Node );

  /*
   *  If a switch handler is present, append it to the switch chain.
   */

  if ( the_extension->Callouts.thread_switch != NULL ) {
    the_extension->Switch.thread_switch = the_extension->Callouts.thread_switch;
    _Chain_Append( 
      &_User_extensions_Switches_list, &the_extension->Switch.Node );
  }
}

/**
 *  This routine is used to remove a user extension set from the active list.
 */

RTEMS_INLINE_ROUTINE void _User_extensions_Remove_set (
  User_extensions_Control  *the_extension
)
{
  _Chain_Extract( &the_extension->Node );
  
  /*
   * If a switch handler is present, remove it.
   */

  if ( the_extension->Callouts.thread_switch != NULL )
    _Chain_Extract( &the_extension->Switch.Node );
}

/**
 *  This routine is used to invoke the user extension which
 *  is invoked when a context switch occurs.
 */

RTEMS_INLINE_ROUTINE void _User_extensions_Thread_switch (
  Thread_Control *executing,
  Thread_Control *heir
)
{
  Chain_Node                     *the_node;
  User_extensions_Switch_control *the_extension_switch;
  
  for ( the_node = _User_extensions_Switches_list.first ;
        !_Chain_Is_tail( &_User_extensions_Switches_list, the_node ) ;
        the_node = the_node->next ) {

    the_extension_switch = (User_extensions_Switch_control *) the_node;

    (*the_extension_switch->thread_switch)( executing, heir );
  }
}

/**@}*/

#endif
/* end of include file */
