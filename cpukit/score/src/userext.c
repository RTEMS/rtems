/*
 *  User Extension Handler
 *
 *  NOTE: XXX
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/score/userext.h>

/*PAGE
 *
 *  _User_extensions_Thread_create
 */

boolean _User_extensions_Thread_create (
  Thread_Control *the_thread
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
  boolean                  status;
 
  for ( the_node = _User_extensions_List.first ;
        !_Chain_Is_tail( &_User_extensions_List, the_node ) ;
        the_node = the_node->next ) {
 
    the_extension = (User_extensions_Control *) the_node;
 
    if ( the_extension->Callouts.thread_create != NULL ) {
      status = (*the_extension->Callouts.thread_create)(
        _Thread_Executing,
        the_thread
      );
      if ( !status )
        return FALSE;
    }
  }
 
  return TRUE;
}

/*PAGE
 *
 *  _User_extensions_Thread_delete
 */
 
void _User_extensions_Thread_delete (
  Thread_Control *the_thread
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
 
  for ( the_node = _User_extensions_List.last ;
        !_Chain_Is_head( &_User_extensions_List, the_node ) ;
        the_node = the_node->previous ) {
 
    the_extension = (User_extensions_Control *) the_node;
 
    if ( the_extension->Callouts.thread_delete != NULL )
      (*the_extension->Callouts.thread_delete)(
        _Thread_Executing,
        the_thread
      );
  }
}
 
/*PAGE
 *
 *  _User_extensions_Thread_start
 *
 */
 
void _User_extensions_Thread_start (
  Thread_Control *the_thread
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
 
  for ( the_node = _User_extensions_List.first ;
        !_Chain_Is_tail( &_User_extensions_List, the_node ) ;
        the_node = the_node->next ) {
 
    the_extension = (User_extensions_Control *) the_node;
 
    if ( the_extension->Callouts.thread_start != NULL )
      (*the_extension->Callouts.thread_start)(
        _Thread_Executing,
        the_thread
      );
  }
}
 
/*PAGE
 *
 *  _User_extensions_Thread_restart
 *
 */
 
void _User_extensions_Thread_restart (
  Thread_Control *the_thread
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
 
  for ( the_node = _User_extensions_List.first ;
        !_Chain_Is_tail( &_User_extensions_List, the_node ) ;
        the_node = the_node->next ) {
 
    the_extension = (User_extensions_Control *) the_node;
 
    if ( the_extension->Callouts.thread_restart != NULL )
      (*the_extension->Callouts.thread_restart)(
        _Thread_Executing,
        the_thread
      );
  }
}
 
/*PAGE
 *
 *  _User_extensions_Thread_begin
 *
 */
 
void _User_extensions_Thread_begin (
  Thread_Control *executing
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
 
  for ( the_node = _User_extensions_List.first ;
        !_Chain_Is_tail( &_User_extensions_List, the_node ) ;
        the_node = the_node->next ) {
 
    the_extension = (User_extensions_Control *) the_node;
 
    if ( the_extension->Callouts.thread_begin != NULL )
      (*the_extension->Callouts.thread_begin)( executing );
  }
}
 
/*PAGE
 *
 *  _User_extensions_Thread_exitted
 */
 
void _User_extensions_Thread_exitted (
  Thread_Control *executing
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
 
  for ( the_node = _User_extensions_List.last ;
        !_Chain_Is_head( &_User_extensions_List, the_node ) ;
        the_node = the_node->previous ) {
 
    the_extension = (User_extensions_Control *) the_node;
 
    if ( the_extension->Callouts.thread_exitted != NULL )
      (*the_extension->Callouts.thread_exitted)( executing );
  }
}

/*PAGE
 *
 *  _User_extensions_Fatal
 */
 
void _User_extensions_Fatal (
  Internal_errors_Source  the_source,
  boolean                 is_internal,
  unsigned32              the_error
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
 
  for ( the_node = _User_extensions_List.last ;
        !_Chain_Is_head( &_User_extensions_List, the_node ) ;
        the_node = the_node->previous ) {
 
    the_extension = (User_extensions_Control *) the_node;
 
    if ( the_extension->Callouts.fatal != NULL )
      (*the_extension->Callouts.fatal)( the_source, is_internal, the_error );
  }
}
 

