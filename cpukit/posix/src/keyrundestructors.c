/*
 *  Copyright (c) 2010 embedded brains GmbH.
 *
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
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/key.h>
#include <rtems/posix/threadsup.h>

/*
 *  _POSIX_Keys_Run_destructors
 *
 *  17.1.1 Thread-Specific Data Key Create, P1003.1c/Draft 10, p. 163
 *
 *  NOTE:  This is the routine executed when a thread exits to
 *         run through all the keys and do the destructor action.
 */

void _POSIX_Keys_Run_destructors(
  Thread_Control *thread
)
{
  POSIX_Keys_List_node *p, *q;
  void *value;
  void (*destructor) (void *);
  POSIX_Keys_Control *the_key;
  Objects_Locations location;
  
  p = ((POSIX_API_Control *)(thread->API_Extensions[ THREAD_API_POSIX ]))->Head;

  while ( p != NULL ) {
    value = p->Rbnode->Value;
    /** problem: this operation should take time...*/
    the_key = _POSIX_Keys_Get( p->Rbnode->Key, &location );
    destructor = the_key->destructor;
    if ( destructor != NULL )
      (*destructor)( value );

    /** delete the node from list */
    q = p->Next;
    _Workspace_Free( p );
    p = q;
  }

  ((POSIX_API_Control *)(thread->API_Extensions[ THREAD_API_POSIX ]))->Head = NULL;
  _Thread_Enable_dispatch();
}
