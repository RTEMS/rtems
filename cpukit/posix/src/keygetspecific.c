/**
 * @file
 *
 * @brief Thread-Specific Data Management
 * @ingroup POSIXAPI
 */

/*
 * Copyright (c) 2012 Zhongwei Yao.
 * COPYRIGHT (c) 1989-2007.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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
#include <rtems/posix/keyimpl.h>

/*
 *  17.1.2 Thread-Specific Data Management, P1003.1c/Draft 10, p. 165
 */

void *pthread_getspecific(
  pthread_key_t  key
)
{
  POSIX_Keys_Control          *the_key;
  Objects_Locations            location;
  POSIX_Keys_Key_value_pair    search_node;
  RBTree_Node                 *p;
  void                        *key_data;
  POSIX_Keys_Key_value_pair   *value_pair_p;

  the_key = _POSIX_Keys_Get( key, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      search_node.key = key;
      search_node.thread_id = _Thread_Executing->Object.id;
      p = _RBTree_Find_unprotected( &_POSIX_Keys_Key_value_lookup_tree,
                                    &search_node.Key_value_lookup_node );
      key_data = NULL;
      if ( p ) {
        value_pair_p = _RBTree_Container_of( p,
                                          POSIX_Keys_Key_value_pair,
                                          Key_value_lookup_node );
        /* key_data = _RBTree_Container_of( p, */
        /*                                  POSIX_Keys_Key_value_pair, */
        /*                                  Key_value_lookup_node )->value; */
        key_data = value_pair_p->value;
      }

      _Objects_Put( &the_key->Object );

      return key_data;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:   /* should never happen */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return NULL;
}
