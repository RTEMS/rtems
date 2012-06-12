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

int pthread_setspecific(
  pthread_key_t  key,
  const void    *value
)
{
  register POSIX_Keys_Control *the_key;
  Objects_Locations            location;
  POSIX_Keys_Rbtree_node *node;

  the_key = _POSIX_Keys_Get( key, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      node = _Workspace_Allocate( sizeof( POSIX_Keys_Rbtree_node ) );
      if ( !node )
	/* problem: what should pthread_setspecific return? */
	return ENOMEM;
      
      node->Key = key;
      node->Thread_id = _Thread_Executing->Object.id;
      node->Value = value;
      /**
       *  it disables interrupts to  ensure the atomicity
       *  of the extract operation. There also is a _RBTree_Insert_unprotected()
       */
      if (!_RBTree_Insert( &_POSIX_Keys_Rbtree, &node.Node ) )
	{
	  _Workspace_Free( node );
	  /* problem: what should pthread_setspecific return? */
	  return EAGAIN;
	}
      
      /* problem: where is the corresponding _Thread_Disable_dispatch()? */
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
