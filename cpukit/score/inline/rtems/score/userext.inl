/*  userext.inl
 *
 *  This file contains the macro implementation of the inlined routines
 *  from the User Extension Handler
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

#ifndef __USER_EXTENSIONS_inl
#define __USER_EXTENSIONS_inl

/*PAGE
 *
 *  _User_extensions_Handler_initialization
 *
 */

STATIC INLINE void _User_extensions_Handler_initialization (
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
 */

STATIC INLINE void _User_extensions_Add_set (
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
 */
 
STATIC INLINE void _User_extensions_Add_API_set (
  User_extensions_Control *the_extension
)
{
  _Chain_Prepend( &_User_extensions_List, &the_extension->Node );
}
 
/*PAGE
 *
 *  _User_extensions_Remove_set
 */

STATIC INLINE void _User_extensions_Remove_set (
  User_extensions_Control  *the_extension
)
{
  _Chain_Extract( &the_extension->Node );
}

/*PAGE
 *
 *  _User_extensions_Thread_switch
 *
 */

STATIC INLINE void _User_extensions_Thread_switch (
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

/*PAGE
 *
 *  _User_extensions_Thread_post_switch
 *
 */
 
STATIC INLINE void _User_extensions_Thread_post_switch (
  Thread_Control *executing
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
 
  for ( the_node = _User_extensions_List.first ;
        !_Chain_Is_tail( &_User_extensions_List, the_node ) ;
        the_node = the_node->next ) {
 
    the_extension = (User_extensions_Control *) the_node;
 
    if ( the_extension->Callouts.thread_post_switch != NULL )
      (*the_extension->Callouts.thread_post_switch)( executing );
  }
}

#endif
/* end of include file */
