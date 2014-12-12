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

static int _POSIX_Keys_Set_value(
  pthread_key_t       key,
  const void         *value,
  POSIX_Keys_Control *the_key,
  Thread_Control     *executing,
  RBTree_Node        *rb_node
)
{
  POSIX_Keys_Key_value_pair *key_value_pair;

  if ( rb_node != NULL ) {
    key_value_pair = POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( rb_node );
    key_value_pair->value = RTEMS_DECONST( void *, value );
  } else {
    key_value_pair = _POSIX_Keys_Key_value_pair_allocate();

    if ( key_value_pair == NULL ) {
      return ENOMEM;
    }

    key_value_pair->key = key;
    key_value_pair->thread = executing;
    key_value_pair->value = RTEMS_DECONST( void *, value );

    /*
     * The insert can only go wrong if the same node is already in a unique
     * tree.  This has been already checked with the _RBTree_Find().
     */
    _RBTree_Insert(
      &_POSIX_Keys_Key_value_lookup_tree,
      &key_value_pair->Key_value_lookup_node,
      _POSIX_Keys_Key_value_compare,
      true
    );

    _Chain_Append_unprotected(
      &executing->Key_Chain,
      &key_value_pair->Key_values_per_thread_node
    );
  }

  return 0;
}

static int _POSIX_Keys_Delete_value(
  pthread_key_t       key,
  POSIX_Keys_Control *the_key,
  RBTree_Node        *rb_node
)
{

  if ( rb_node != NULL ) {
    POSIX_Keys_Key_value_pair *key_value_pair =
      POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( rb_node );

    _POSIX_Keys_Free_key_value_pair( key_value_pair );
  }

  return 0;
}

/*
 *  17.1.2 Thread-Specific Data Management, P1003.1c/Draft 10, p. 165
 */

int pthread_setspecific(
  pthread_key_t  key,
  const void    *value
)
{
  POSIX_Keys_Control *the_key;
  Objects_Locations   location;
  Thread_Control     *executing;
  RBTree_Node        *rb_node;
  int                 eno;

  the_key = _POSIX_Keys_Get( key, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      executing = _Thread_Executing;
      rb_node = _POSIX_Keys_Find( key, executing );

      if ( value != NULL ) {
        eno = _POSIX_Keys_Set_value( key, value, the_key, executing, rb_node );
      } else {
        eno = _POSIX_Keys_Delete_value( key, the_key, rb_node );
      }

      _Objects_Put( &the_key->Object );

      return eno;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:   /* should never happen */
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
