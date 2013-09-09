/**
 * @file
 *
 * @brief POSIX Keys Manager Initialization
 * @ingroup POSIX_KEY Key
 */

/*
 * Copyright (c) 2012 Zhongwei Yao.
 * COPYRIGHT (c) 1989-2008.
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
#include <rtems/posix/config.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/wkspace.h>

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

int _POSIX_Keys_Key_value_lookup_tree_compare_function(
  const RBTree_Node *node1,
  const RBTree_Node *node2
)
{
  POSIX_Keys_Key_value_pair *n1;
  POSIX_Keys_Key_value_pair *n2;
  Objects_Id thread_id1, thread_id2;
  int diff;

  n1 = _RBTree_Container_of( node1, POSIX_Keys_Key_value_pair, Key_value_lookup_node );
  n2 = _RBTree_Container_of( node2, POSIX_Keys_Key_value_pair, Key_value_lookup_node );

  diff = n1->key - n2->key;
  if ( diff )
    return diff;

  thread_id1 = n1->thread_id;
  thread_id2 = n2->thread_id;

  /**
   * if thread_id1 or thread_id2 equals to 0, only key1 and key2 is valued.
   * it enables us search node only by pthread_key_t type key.
   */
  if ( thread_id1 && thread_id2 )
    return thread_id1 - thread_id2;
  return 0;
}

static uint32_t _POSIX_Keys_Get_keypool_bump_count( void )
{
  uint32_t max = Configuration_POSIX_API.maximum_key_value_pairs;

  return _Objects_Is_unlimited( max ) ?
    _Objects_Maximum_per_allocation( max ) : 0;
}

static uint32_t _POSIX_Keys_Get_initial_keypool_size( void )
{
  uint32_t max = Configuration_POSIX_API.maximum_key_value_pairs;

  return _Objects_Maximum_per_allocation( max );
}

static bool _POSIX_Keys_Keypool_extend( Freechain_Control *keypool )
{
  size_t bump_count = _POSIX_Keys_Get_keypool_bump_count();
  bool ok = bump_count > 0;

  if ( ok ) {
    size_t size = bump_count * sizeof( POSIX_Keys_Key_value_pair );
    POSIX_Keys_Key_value_pair *nodes = _Workspace_Allocate( size );

    ok = nodes != NULL;

    if ( ok ) {
      _Chain_Initialize(
        &keypool->Freechain,
        nodes,
        bump_count,
        sizeof( *nodes )
      );
    }
  }

  return ok;
}

static void _POSIX_Keys_Initialize_keypool( void )
{
  Freechain_Control *keypool = &_POSIX_Keys_Keypool;
  size_t initial_count = _POSIX_Keys_Get_initial_keypool_size();
  size_t size = initial_count * sizeof( POSIX_Keys_Key_value_pair );
  POSIX_Keys_Key_value_pair *nodes = _Workspace_Allocate_or_fatal_error( size );

  _Freechain_Initialize( keypool, _POSIX_Keys_Keypool_extend );

  _Chain_Initialize(
    &keypool->Freechain,
    nodes,
    initial_count,
    sizeof( *nodes )
  );
}

/**
 * @brief This routine performs the initialization necessary for this manager.
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
      &_POSIX_Keys_Key_value_lookup_tree,
      _POSIX_Keys_Key_value_lookup_tree_compare_function,
      true
  );

  _POSIX_Keys_Initialize_keypool();
}
