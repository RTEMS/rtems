/*  userext.inl
 *
 *  This file contains the macro implementation of the inlined routines
 *  from the User Extension Handler
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
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
 */

#define _User_extensions_Handler_initialization( \
  number_of_extensions, _initial_extensions \
) \
  { \
    User_extensions_Control *extension; \
    unsigned32               i; \
    _Chain_Initialize_empty( &_User_extensions_List ); \
    \
    if ( (_initial_extensions) ) { \
      for (i=0 ; i<number_of_extensions ; i++ ) { \
        extension = \
           _Workspace_Allocate_or_fatal_error( sizeof(User_extensions_Control) ); \
        \
        extension->Callouts = _initial_extensions[i]; \
        _Chain_Append( &_User_extensions_List, &extension->Node ); \
      } \
    } \
  }

/*PAGE
 *
 *  _User_extensions_Add_set
 */

#define _User_extensions_Add_set( _the_extension, _extension_table ) \
  do { \
    (_the_extension)->Callouts = *(_extension_table); \
    \
    _Chain_Append( &_User_extensions_List, &(_the_extension)->Node ); \
  } while ( 0 )

/*PAGE
 *
 *  _User_extensions_Add_API_set
 */
 
#define _User_extensions_Add_API_set( _the_extension ) \
  _Chain_Prepend( &_User_extensions_List, &(_the_extension)->Node )
 

/*PAGE
 *
 *  _User_extensions_Remove_set
 */

#define _User_extensions_Remove_set( _the_extension ) \
  _Chain_Extract( &(_the_extension)->Node )

/*PAGE
 *
 *  _User_extensions_Run_list_forward
 *
 *  NOTE:  No parentheses around macro names here to avoid
 *         messing up the name and function call expansion.
 */

#define _User_extensions_Run_list_forward( _name, _arguments ) \
  do { \
    Chain_Node              *the_node; \
    User_extensions_Control *the_extension; \
    \
    for ( the_node = _User_extensions_List.first ; \
          !_Chain_Is_tail( &_User_extensions_List, the_node ) ; \
          the_node = the_node->next ) { \
      the_extension = (User_extensions_Control *) the_node; \
      \
      if ( the_extension->Callouts.## _name != NULL ) \
        (*the_extension->Callouts.## _name) _arguments; \
      \
    } \
    \
  } while ( 0 )

/*PAGE
 *
 *  _User_extensions_Run_list_backward
 *
 *  NOTE:  No parentheses around macro names here to avoid
 *         messing up the name and function call expansion.
 */

#define _User_extensions_Run_list_backward( _name, _arguments ) \
  do { \
    Chain_Node              *the_node; \
    User_extensions_Control *the_extension; \
    \
    for ( the_node = _User_extensions_List.last ; \
          !_Chain_Is_head( &_User_extensions_List, the_node ) ; \
          the_node = the_node->previous ) { \
      the_extension = (User_extensions_Control *) the_node; \
      \
      if ( the_extension->Callouts.## _name != NULL ) \
        (*the_extension->Callouts.## _name) _arguments; \
      \
    } \
    \
  } while ( 0 )

/*PAGE
 *
 *  _User_extensions_Thread_switch
 *
 */

#define _User_extensions_Thread_switch( _executing, _heir ) \
  _User_extensions_Run_list_forward(thread_switch, (_executing, _heir) )

#endif
/* end of include file */
