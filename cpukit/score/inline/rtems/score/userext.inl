/*  userext.inl
 *
 *  This file contains the macro implementation of the inlined routines
 *  from the User Extension Handler
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __USER_EXTENSIONS_inl
#define __USER_EXTENSIONS_inl

/*PAGE
 *
 *  _User_extensions_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this handler.
 */

RTEMS_INLINE_ROUTINE void _User_extensions_Handler_initialization (
    User_extensions_Table  *initial_extensions
)
{
  _Chain_Initialize_empty( &_User_extensions_List );

  if ( initial_extensions ) {
    _User_extensions_Initial.Callouts = *initial_extensions;
    _Chain_Append( &_User_extensions_List, &_User_extensions_Initial.Node );
  }
}

/*PAGE
 *
 *  _User_extensions_Add_set
 *
 *  DESCRIPTION:
 *
 *  This routine is used to add a user extension set to the active list.
 */

RTEMS_INLINE_ROUTINE void _User_extensions_Add_set (
  User_extensions_Control *the_extension,
  User_extensions_Table   *extension_table
)
{
  the_extension->Callouts = *extension_table;

  _Chain_Append( &_User_extensions_List, &the_extension->Node );
}

/*PAGE
 *
 *  _User_extensions_Add_API_set
 *  DESCRIPTION:
 *
 *  This routine is used to add an API extension set to the active list.
 */
 
RTEMS_INLINE_ROUTINE void _User_extensions_Add_API_set (
  User_extensions_Control *the_extension
)
{
  _Chain_Prepend( &_User_extensions_List, &the_extension->Node );
}
 
/*PAGE
 *
 *  _User_extensions_Remove_set
 *
 *  DESCRIPTION:
 *
 *  This routine is used to remove a user extension set from the active list.
 */

RTEMS_INLINE_ROUTINE void _User_extensions_Remove_set (
  User_extensions_Control  *the_extension
)
{
  _Chain_Extract( &the_extension->Node );
}

/*PAGE
 *
 *  _User_extensions_Thread_switch
 *
 *  DESCRIPTION:
 *
 *  This routine is used to invoke the user extension which
 *  is invoked when a context switch occurs.
 */

RTEMS_INLINE_ROUTINE void _User_extensions_Thread_switch (
  Thread_Control *executing,
  Thread_Control *heir
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;

  for ( the_node = _User_extensions_List.first ;
        !_Chain_Is_tail( &_User_extensions_List, the_node ) ;
        the_node = the_node->next ) {

    the_extension = (User_extensions_Control *) the_node;

    if ( the_extension->Callouts.thread_switch != NULL )
      (*the_extension->Callouts.thread_switch)( executing, heir );
  }
}

#endif
/* end of include file */
