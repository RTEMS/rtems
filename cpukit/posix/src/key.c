/*
 *  COPYRIGHT (c) 1989-2008.
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
#include <limits.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/key.h>
#include <rtems/score/rbtree.h>

/*
 * _POSIX_Key_Rbtree_Compare_Function
 *
 * DESCRIPTION:
 * This routine compares the rbtree node
 * by comparing POSIX key first and comparing thread id second 
 *
 * Input parameters: two rbtree node
 *
 * Output parameters: return 1 if first node
 * has higher key than second, -1 if lower, 0 if equal,
 * and for all the thread id is unique, then return 0 is impossible
 */

int _POSIX_Key_Rbtree_Compare_Function(
  const Rbtree_Node *node1,
  const Rbtree_Node *node2
)
{
  pthread_key_t key1 = _RBTree_Container_of(node1, POSIX_Keys_Rbtree_Node, Node)->Key;
  pthread_key_t key2 = _RBTree_Container_of(node2, POSIX_Keys_Rbtree_Node, Node)->Key;

  Object_Id thread_id1 = _RBTree_Container_of(node1, POSIX_Keys_Rbtree_Node, Node)->Thread_id;
  Object_Id thread_id2 = _RBTree_Container_of(node2, POSIX_Keys_Rbtree_Node, Node)->Thread_id;
  
  if ( key1 == key2 )
    {
      if (thread_id1 > thread_id2)
	return 1;
      else
	return -1;
    }
  else if ( key1 > key2 )
    return 1;
  else
    return -1;
}
  

/*
 *  _POSIX_Key_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _POSIX_Key_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_POSIX_Keys_Information,   /* object information table */
    OBJECTS_POSIX_API,          /* object API */
    OBJECTS_POSIX_KEYS,         /* object class */
    Configuration_POSIX_API.maximum_keys,
                                /* maximum objects of this class */
    sizeof( POSIX_Keys_Control ),
                                /* size of this object's control block */
    true,                       /* true if names for this object are strings */
    _POSIX_PATH_MAX             /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    false,                      /* true if this is a global object class */
    NULL                        /* Proxy extraction support callout */
#endif
  );

  _RBTree_Initialize_empty( 
    _POSIX_Keys_Rbtree,     /* the rbtree control block */
    _POSIX_Key_Rbtree_Compare_Function,
                            /* the rbtree compare function */
    true                    /* true if each rbtree node is unique */
  );
}
