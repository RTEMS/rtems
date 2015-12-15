/**
 * @file
 *
 * @brief POSIX Keys Manager Initialization
 * @ingroup POSIX_KEY Key
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

#include <rtems/config.h>
#include <rtems/sysinit.h>

#include <rtems/posix/keyimpl.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/wkspace.h>

Objects_Information _POSIX_Keys_Information;

RBTREE_DEFINE_EMPTY( _POSIX_Keys_Key_value_lookup_tree );

Freechain_Control _POSIX_Keys_Keypool;

/**
 * @brief This routine compares the rbtree node by comparing POSIX key first
 * and comparing thread id second.
 *
 * if either of the input nodes's thread_id member is 0, then it will only
 * compare the pthread_key_t member. That is when we pass thread_id = 0 node
 * as a search node, the search is done only by pthread_key_t.
 *
 * @param[in] node1 The node to be compared
 * @param[in] node2 The node to be compared
 * @retval positive if first node has higher key than second
 * @retval negative if lower
 * @retval 0 if equal,and for all the thread id is unique, then return 0 is
 * impossible
 */

RBTree_Compare_result _POSIX_Keys_Key_value_compare(
  const RBTree_Node *node1,
  const RBTree_Node *node2
)
{
  POSIX_Keys_Key_value_pair *n1;
  POSIX_Keys_Key_value_pair *n2;
  Thread_Control *thread1;
  Thread_Control *thread2;
  RBTree_Compare_result diff;

  n1 = POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( node1 );
  n2 = POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( node2 );

  diff = n1->key - n2->key;
  if ( diff )
    return diff;

  thread1 = n1->thread;
  thread2 = n2->thread;

  /*
   * If thread1 or thread2 equals to NULL, only key1 and key2 is valued.  It
   * enables us search node only by pthread_key_t type key.  Exploit that the
   * thread control alignment is at least two to avoid integer overflows.
   */
  if ( thread1 != NULL && thread2 != NULL )
    return (RBTree_Compare_result) ( (uintptr_t) thread1 >> 1 )
      - (RBTree_Compare_result) ( (uintptr_t) thread2 >> 1 );

  return 0;
}

static uint32_t _POSIX_Keys_Get_keypool_bump_count( void )
{
  uint32_t max = Configuration.maximum_key_value_pairs;

  return _Objects_Is_unlimited( max ) ?
    _Objects_Maximum_per_allocation( max ) : 0;
}

static uint32_t _POSIX_Keys_Get_initial_keypool_size( void )
{
  uint32_t max = Configuration.maximum_key_value_pairs;

  return _Objects_Maximum_per_allocation( max );
}

static void _POSIX_Keys_Initialize_keypool( void )
{
  _Freechain_Initialize(
    &_POSIX_Keys_Keypool,
    _Workspace_Allocate_or_fatal_error,
    _POSIX_Keys_Get_initial_keypool_size(),
    sizeof( POSIX_Keys_Key_value_pair )
  );
}

POSIX_Keys_Key_value_pair * _POSIX_Keys_Key_value_pair_allocate( void )
{
  return (POSIX_Keys_Key_value_pair *) _Freechain_Get(
    &_POSIX_Keys_Keypool,
    _Workspace_Allocate,
    _POSIX_Keys_Get_keypool_bump_count(),
    sizeof( POSIX_Keys_Key_value_pair )
  );
}

static void _POSIX_Keys_Run_destructors( Thread_Control *thread )
{
  Chain_Control *chain;
  POSIX_Keys_Key_value_pair *iter, *next;
  void *value;
  void (*destructor) (void *);
  POSIX_Keys_Control *the_key;
  Objects_Locations location;

  chain = &thread->Key_Chain;
  iter = (POSIX_Keys_Key_value_pair *) _Chain_First( chain );
  while ( !_Chain_Is_tail( chain, &iter->Key_values_per_thread_node ) ) {
    next = (POSIX_Keys_Key_value_pair *)
      _Chain_Next( &iter->Key_values_per_thread_node );

    the_key = _POSIX_Keys_Get( iter->key, &location );
    _Assert( location == OBJECTS_LOCAL );

    /**
     * remove key from rbtree and chain.
     * here Chain_Node *iter can be convert to POSIX_Keys_Key_value_pair *,
     * because Chain_Node is the first member of POSIX_Keys_Key_value_pair
     * structure.
     */
    _RBTree_Extract(
        &_POSIX_Keys_Key_value_lookup_tree,
        &iter->Key_value_lookup_node
    );
    _Chain_Extract_unprotected( &iter->Key_values_per_thread_node );

    destructor = the_key->destructor;
    value = iter->value;

    _POSIX_Keys_Key_value_pair_free( iter );

    _Objects_Put( &the_key->Object );

    /**
     * run key value's destructor if destructor and value are both non-null.
     */
    if ( destructor != NULL && value != NULL )
      (*destructor)( value );

    iter = next;
  }
}

static User_extensions_Control _POSIX_Keys_Extensions = {
  .Callouts = {
    .thread_terminate = _POSIX_Keys_Run_destructors
  }
};

/**
 * @brief This routine performs the initialization necessary for this manager.
 */
static void _POSIX_Keys_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_POSIX_Keys_Information,   /* object information table */
    OBJECTS_POSIX_API,          /* object API */
    OBJECTS_POSIX_KEYS,         /* object class */
    Configuration.maximum_keys,
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

  _POSIX_Keys_Initialize_keypool();

  _User_extensions_Add_API_set( &_POSIX_Keys_Extensions );
}

RTEMS_SYSINIT_ITEM(
  _POSIX_Keys_Manager_initialization,
  RTEMS_SYSINIT_POSIX_KEYS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
