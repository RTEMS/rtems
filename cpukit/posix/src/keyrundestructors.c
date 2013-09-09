/**
 * @file
 *
 * @brief Thread-Specific Data Key Create
 * @ingroup POSIX_KEY Key
 */

/*
 * Copyright (c) 2012 Zhongwei Yao.
 * Copyright (c) 2010 embedded brains GmbH.
 *
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

#include <rtems/posix/keyimpl.h>
#include <rtems/posix/threadsup.h>
#include <rtems/score/chainimpl.h>

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
  POSIX_Keys_Key_value_pair *iter, *next;
  void *value;
  void (*destructor) (void *);
  POSIX_Keys_Control *the_key;
  Objects_Locations location;

  _Thread_Disable_dispatch();

  chain = &(
      (POSIX_API_Control *)thread->API_Extensions[ THREAD_API_POSIX ]
  )->Key_Chain;
  iter = (POSIX_Keys_Key_value_pair *) _Chain_First( chain );
  while ( !_Chain_Is_tail( chain, &iter->Key_values_per_thread_node ) ) {
    next = (POSIX_Keys_Key_value_pair *)
      _Chain_Next( &iter->Key_values_per_thread_node );

    /**
     * remove key from rbtree and chain.
     * here Chain_Node *iter can be convert to POSIX_Keys_Key_value_pair *,
     * because Chain_Node is the first member of POSIX_Keys_Key_value_pair
     * structure.
     */
    _RBTree_Extract_unprotected(
        &_POSIX_Keys_Key_value_lookup_tree,
        &iter->Key_value_lookup_node
    );
    _Chain_Extract_unprotected( &iter->Key_values_per_thread_node );

    /**
     * run key value's destructor if destructor and value are both non-null.
     */
    the_key = _POSIX_Keys_Get( iter->key, &location );
    destructor = the_key->destructor;
    value = iter->value;
    if ( destructor != NULL && value != NULL )
      (*destructor)( value );

    _Objects_Put( &the_key->Object );

    _POSIX_Keys_Key_value_pair_free( iter );

    iter = next;
  }

  _Thread_Enable_dispatch();
}
