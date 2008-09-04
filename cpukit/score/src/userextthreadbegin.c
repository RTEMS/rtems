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
  bool                    is_internal,
  uint32_t                the_error
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
