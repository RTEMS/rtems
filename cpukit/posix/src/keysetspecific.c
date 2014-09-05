/**
 * @file
 *
 * @brief Set Specific Key
 * @ingroup POSIXAPI
 */

/*
 * Copyright (c) 2012 Zhongwei Yao.
 * COPYRIGHT (c) 1989-2014.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/keyimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/chainimpl.h>

#include <errno.h>

/*
 *  17.1.2 Thread-Specific Data Management, P1003.1c/Draft 10, p. 165
 */

int pthread_setspecific(
  pthread_key_t  key,
  const void    *value
)
{
  POSIX_Keys_Control          *the_key;
  Objects_Locations            location;
  POSIX_Keys_Key_value_pair   *value_pair_ptr;
  RBTree_Node                 *p;
  POSIX_Keys_Key_value_pair    search_node;
  Thread_Control              *executing;

  the_key = _POSIX_Keys_Get( key, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      executing = _Thread_Executing;
      p = _POSIX_Keys_Find( key, executing, &search_node );
      if ( p != NULL ) {
        value_pair_ptr = POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( p );
        value_pair_ptr->value = RTEMS_DECONST( void *, value );
      } else {
        value_pair_ptr = _POSIX_Keys_Key_value_pair_allocate();

        if ( !value_pair_ptr ) {
          _Objects_Put( &the_key->Object );

          return ENOMEM;
        }

        value_pair_ptr->key = key;
        value_pair_ptr->thread = executing;
        value_pair_ptr->value = RTEMS_DECONST( void *, value );
        /* The insert can only go wrong if the same node is already in a unique
         * tree. This has been already checked with the _RBTree_Find() */
        _RBTree_Insert(
          &_POSIX_Keys_Key_value_lookup_tree,
          &value_pair_ptr->Key_value_lookup_node,
          _POSIX_Keys_Key_value_compare,
          true
        );

        /** append rb_node to the thread API extension's chain */
        _Chain_Append_unprotected(
          &_Thread_Executing->Key_Chain,
          &value_pair_ptr->Key_values_per_thread_node
        );
      }

      _Objects_Put( &the_key->Object );

      return 0;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:   /* should never happen */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
