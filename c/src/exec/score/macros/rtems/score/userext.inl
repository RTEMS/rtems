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

#include <rtems/score/wkspace.h>

/*PAGE
 *
 *  _User_extensions_Add_set
 *
 *  NOTE: Must be before _User_extensions_Handler_initialization to
 *        ensure proper inlining.
 */
 
#define _User_extensions_Add_set( _the_extension, _extension_table ) \
  do { \
    (_the_extension)->Callouts = *(_extension_table); \
    \
    _Chain_Append( &_User_extensions_List, &(_the_extension)->Node ); \
    \
    if ( (_the_extension)->Callouts.thread_switch != NULL ) { \
      (_the_extension)->Switch.thread_switch = \
        (_the_extension)->Callouts.thread_switch; \
      _Chain_Append( \
        &_User_extensions_Switches_list, \
        &(_the_extension)->Switch.Node \
     ); \
    } \
  } while ( 0 )
 

/*PAGE
 *
 *  _User_extensions_Handler_initialization
 *
 */

#define _User_extensions_Handler_initialization( \
  _number_of_extensions, _initial_extensions \
) \
  { \
    User_extensions_Control *extension; \
    unsigned32               i; \
    \
    _Chain_Initialize_empty( &_User_extensions_List ); \
    _Chain_Initialize_empty( &_User_extensions_Switches_list ); \
    \
    if ( (_initial_extensions) ) { \
      extension = _Workspace_Allocate_or_fatal_error( \
          sizeof(User_extensions_Control) * _number_of_extensions ); \
      \
      memset ( \
        extension, \
        0, \
        _number_of_extensions * sizeof( User_extensions_Control ) \
      ); \
      \
      for ( i = 0 ; i < _number_of_extensions ; i++ ) { \
        _User_extensions_Add_set (extension, &_initial_extensions[i]); \
        extension++; \
      } \
    } \
  }

/*PAGE
 *
 *  _User_extensions_Add_API_set
 */
 
#define _User_extensions_Add_API_set( _the_extension ) \
  do { \
    _Chain_Append( &_User_extensions_List, &(_the_extension)->Node ); \
    \
    if ( (_the_extension)->Callouts.thread_switch != NULL ) { \
      (_the_extension)->Switch.thread_switch = \
          (_the_extension)->Callouts.thread_switch; \
      _Chain_Append( \
        &_User_extensions_Switches_list, &(_the_extension)->Switch.Node ); \
    } \
  } while ( 0 )
 
/*PAGE
 *
 *  _User_extensions_Remove_set
 */

#define _User_extensions_Remove_set( _the_extension ) \
  do { \
    _Chain_Extract( &(_the_extension)->Node ); \
    \
    if ( (_the_extension)->Callouts.thread_switch != NULL ) { \
      _Chain_Extract( &(_the_extension)->Node ); \
    } \
  } while (0)

/*PAGE
 *
 *  _User_extensions_Thread_switch
 *
 */

#define _User_extensions_Thread_switch( _executing, _heir ) \
  do { \
    Chain_Node                     *the_node; \
    User_extensions_Switch_control *the_extension_switch; \
    \
    for ( the_node = _User_extensions_Switches_list.first ; \
          !_Chain_Is_tail( &_User_extensions_Switches_list, the_node ) ; \
          the_node = the_node->next ) { \
      \
      the_extension_switch = (User_extensions_Switch_control *) the_node; \
      \
      (*the_extension_switch->thread_switch)( _executing, _heir ); \
    } \
  } while (0)

#endif
/* end of include file */
