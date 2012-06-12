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
  register POSIX_Keys_Control *the_key;
  Objects_Locations            location;
  POSIX_Keys_Rbtree_node      search_node;
  POSIX_Keys_Rbtree_node     *p; 

  the_key = _POSIX_Keys_Get( key, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      search_node.Key = key;
      search_node.Thread_id = _Thread_Executing->Object.id;
      p = _RBTree_Find( &_POSIX_Keys_Rbtree, &search_node.Node);
      if ( !p )
	return NULL;
      /* problem: where is the corresponding _Thread_Disable_dispatch()? */
      _Thread_Enable_dispatch();
      return p->Value;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:   /* should never happen */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return NULL;
}
