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
  Objects_Locations            location;
  POSIX_Keys_Rbtree_node      *rb_node;
  POSIX_API_Control           *api;

  /** _POSIX_Keys_Get() would call _Thread_Disable_dispatch() implicitly*/
  _POSIX_Keys_Get( key, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      rb_node = _Workspace_Allocate( sizeof( POSIX_Keys_Rbtree_node ) );
      if ( !rb_node ) {
        _Thread_Enable_dispatch();
        return ENOMEM;
      }
      
      rb_node->key = key;
      rb_node->thread_id = _Thread_Executing->Object.id;
      rb_node->value = value;
      if (_RBTree_Insert_unprotected( &_POSIX_Keys_Rbtree, &(rb_node->rb_node) ) ) {
          _Workspace_Free( rb_node );
          _Thread_Enable_dispatch();
          return EAGAIN;
        }
      
      /** append rb_node to the thread API extension's chain */
      api = (POSIX_API_Control *)(_Thread_Executing->API_Extensions[THREAD_API_POSIX]);
      _Chain_Append_unprotected( &api->the_chain, &rb_node->ch_node );
      
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
