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
