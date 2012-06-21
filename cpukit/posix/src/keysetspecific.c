/*
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

#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/rbtree.h>
#include <rtems/posix/key.h>
#include <rtems/posix/threadsup.h>

/*
 *  17.1.2 Thread-Specific Data Management, P1003.1c/Draft 10, p. 165
 */

int pthread_setspecific(
  pthread_key_t  key,
  const void    *value
)
{
  register POSIX_Keys_Control *the_key;
  Objects_Locations            location;
  POSIX_Keys_Rbtree_node      *rb_node;
  POSIX_Keys_List_node        *lt_node1, *lt_node2;

  /** _POSIX_Keys_Get() would call _Thread_Disable_dispatch()*/
  the_key = _POSIX_Keys_Get( key, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      rb_node = _Workspace_Allocate( sizeof( POSIX_Keys_Rbtree_node ) );
      if ( !rb_node ) {
	_Thread_Enable_dispatch;
	return ENOMEM;
      }
      
      lt_node1 = _Workspace_Allocate( sizeof( POSIX_Keys_List_node ) );
      if ( !lt_node1 ) {
	_Thread_Enable_dispatch;
	_Workspace_Free( rb_node );
	return ENOMEM;
      }
       
      lt_node2 = _Workspace_Allocate( sizeof( POSIX_Keys_List_node ) );
      if ( !lt_node2 ) {
	_Workspace_Free( rb_node );
	_Workspace_Free( lt_node1 );
	_Thread_Enable_dispatch;
	return ENOMEM;
      }
      
      rb_node->key = key;
      rb_node->thread_id = _Thread_Executing->Object.id;
      rb_node->value = value;
      /**
       *  it disables interrupts to  ensure the atomicity
       *  of the extract operation. There also is a _RBTree_Insert_unprotected()
       */
      if (_RBTree_Insert( &_POSIX_Keys_Rbtree, &(rb_node->node) ) ) {
	  _Workspace_Free( rb_node );
	  _Workspace_Free( lt_node1 );
	  _Workspace_Free( lt_node2 );
	  /* problem: what should pthread_setspecific return? */
	  _Thread_Enable_dispatch;
	  return EAGAIN;
	}
      
      /** insert lt_node1 to the POSIX key control's list */
      lt_node1->rbnode = rb_node;
      lt_node1->next = the_key->head;
      the_key->head = lt_node1;
      
      /** insert lt_node2 to the thread API extension's list */
      lt_node2->rbnode = rb_node;
      lt_node2->next = ((POSIX_API_Control *)(_Thread_Executing->API_Extensions[ THREAD_API_POSIX ]))->head;
      ((POSIX_API_Control *)(_Thread_Executing->API_Extensions[ THREAD_API_POSIX ]))->head = lt_node2;
      _Thread_Enable_dispatch();
      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:   /* should never happen */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
