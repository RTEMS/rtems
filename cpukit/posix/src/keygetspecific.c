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

/*
 *  17.1.2 Thread-Specific Data Management, P1003.1c/Draft 10, p. 165
 */

void *pthread_getspecific(
  pthread_key_t  key
)
{
  Objects_Locations            location;
  POSIX_Keys_Rbtree_node       search_node;
  RBTree_Node                 *p; 

  _POSIX_Keys_Get( key, &location );
  switch ( location ) {
    
    case OBJECTS_LOCAL:
      /** TODO: search the node in TCB's chain(maybe the rbtree) to speed up the search */
      search_node.key = key;
      search_node.thread_id = _Thread_Executing->Object.id;
      p = _RBTree_Find_unprotected( &_POSIX_Keys_Rbtree, &search_node.rb_node);
      if ( !p ) {
	_Thread_Enable_dispatch();
	return NULL;
      }
      _Thread_Enable_dispatch();
      return _RBTree_Container_of( p, POSIX_Keys_Rbtree_node, rb_node )->value;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:   /* should never happen */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return NULL;
}
