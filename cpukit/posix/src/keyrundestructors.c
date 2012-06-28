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
#include <rtems/score/chain.h>
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
  Chain_Control *chain;
  Chain_Node *iter, *next;
  void *value;
  void (*destructor) (void *);
  POSIX_Keys_Control *the_key;
  Objects_Locations location;
  
  chain = &((POSIX_API_Control *)thread->API_Extensions[ THREAD_API_POSIX ])->the_chain;
  iter = _Chain_First( chain );
  while ( !_Chain_Is_tail( chain, iter ) ) {
    next = _Chain_Next( iter );
    /**
     * here Chain_Node *iter can be convert to POSIX_Keys_Rbtree_node *,
     * because Chain_Node is the first member of POSIX_Keys_Rbtree_node structure.
     */
    _RBTree_Extract( &_POSIX_Keys_Rbtree, &((POSIX_Keys_Rbtree_node *)iter)->rb_node );
    _Chain_Extract( iter );
    
    the_key = _POSIX_Keys_Get( ((POSIX_Keys_Rbtree_node *)iter)->key, &location);
    destructor = the_key->destructor;
    value = ((POSIX_Keys_Rbtree_node *)iter)->value;
    if ( destructor != NULL && value != NULL )
      (*destructor)( value );
    
    _Workspace_Free( (POSIX_Keys_Rbtree_node *)iter );
    iter = next;
  }
  /* problem: should _Thread_Enable_dispatch be placed in while? */
  _Thread_Enable_dispatch();
}
