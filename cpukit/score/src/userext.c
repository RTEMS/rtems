/*
 *  User Extension Handler
 *
 *  NOTE: XXX
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

#include <rtems/system.h>
#include <rtems/userext.h>

/*PAGE
 *
 *  _User_extensions_Task_create
 */

boolean _User_extensions_Task_create (
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
 
    if ( the_extension->Callouts.rtems_task_create != NULL ) {
      status = (*the_extension->Callouts.rtems_task_create)(
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
 *  _User_extensions_Task_delete
 */
 
void _User_extensions_Task_delete (
  Thread_Control *the_thread
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
 
  for ( the_node = _User_extensions_List.last ;
        !_Chain_Is_head( &_User_extensions_List, the_node ) ;
        the_node = the_node->previous ) {
 
    the_extension = (User_extensions_Control *) the_node;
 
    if ( the_extension->Callouts.rtems_task_delete != NULL )
      (*the_extension->Callouts.rtems_task_delete)(
        _Thread_Executing,
        the_thread
      );
  }
}
 
/*PAGE
 *
 *  _User_extensions_Task_start
 *
 */
 
void _User_extensions_Task_start (
  Thread_Control *the_thread
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
 
  for ( the_node = _User_extensions_List.first ;
        !_Chain_Is_tail( &_User_extensions_List, the_node ) ;
        the_node = the_node->next ) {
 
    the_extension = (User_extensions_Control *) the_node;
 
    if ( the_extension->Callouts.rtems_task_start != NULL )
      (*the_extension->Callouts.rtems_task_start)(
        _Thread_Executing,
        the_thread
      );
  }
}
 
/*PAGE
 *
 *  _User_extensions_Task_restart
 *
 */
 
void _User_extensions_Task_restart (
  Thread_Control *the_thread
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
 
  for ( the_node = _User_extensions_List.first ;
        !_Chain_Is_tail( &_User_extensions_List, the_node ) ;
        the_node = the_node->next ) {
 
    the_extension = (User_extensions_Control *) the_node;
 
    if ( the_extension->Callouts.rtems_task_restart != NULL )
      (*the_extension->Callouts.rtems_task_restart)(
        _Thread_Executing,
        the_thread
      );
  }
}
 
/*PAGE
 *
 *  _User_extensions_Task_begin
 *
 */
 
void _User_extensions_Task_begin (
  Thread_Control *executing
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
 
  for ( the_node = _User_extensions_List.first ;
        !_Chain_Is_tail( &_User_extensions_List, the_node ) ;
        the_node = the_node->next ) {
 
    the_extension = (User_extensions_Control *) the_node;
 
    if ( the_extension->Callouts.task_begin != NULL )
      (*the_extension->Callouts.task_begin)( executing );
  }
}
 
/*PAGE
 *
 *  _User_extensions_Task_exitted
 */
 
void _User_extensions_Task_exitted (
  Thread_Control *executing
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
 
  for ( the_node = _User_extensions_List.last ;
        !_Chain_Is_head( &_User_extensions_List, the_node ) ;
        the_node = the_node->previous ) {
 
    the_extension = (User_extensions_Control *) the_node;
 
    if ( the_extension->Callouts.task_exitted != NULL )
      (*the_extension->Callouts.task_exitted)( executing );
  }
}

/*PAGE
 *
 *  _User_extensions_Fatal
 */
 
void _User_extensions_Fatal (
  unsigned32 the_error
)
{
  Chain_Node              *the_node;
  User_extensions_Control *the_extension;
 
  for ( the_node = _User_extensions_List.last ;
        !_Chain_Is_head( &_User_extensions_List, the_node ) ;
        the_node = the_node->previous ) {
 
    the_extension = (User_extensions_Control *) the_node;
 
    if ( the_extension->Callouts.fatal != NULL )
      (*the_extension->Callouts.fatal)( the_error );
  }
}
 

