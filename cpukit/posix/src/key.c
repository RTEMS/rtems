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

/**
 * @brief This routine does user side freechain initialization
 */
static void _POSIX_Keys_Freechain_init(Freechain_Control *freechain)
{
  POSIX_Keys_Freechain *psx_freechain_p = (POSIX_Keys_Freechain *)freechain;
  psx_freechain_p->bump_count =
    Configuration_POSIX_API.maximum_key_value_pairs & 0x7FFFFFFF;
  size_t size = psx_freechain_p->bump_count * sizeof(POSIX_Keys_Key_value_pair);
  POSIX_Keys_Key_value_pair *nodes = _Workspace_Allocate(size);

  _Chain_Initialize(
                    &freechain->Freechain,
                    nodes,
                    psx_freechain_p->bump_count,
                    sizeof(POSIX_Keys_Key_value_pair)
                    );
}

/**
 * @brief This routine does keypool initialize, keypool contains all
 * POSIX_Keys_Key_value_pair
 */

static void _POSIX_Keys_Keypool_init(void)
{
  _Freechain_Initialize((Freechain_Control *)&_POSIX_Keys_Keypool,
                       &_POSIX_Keys_Freechain_extend);

  _POSIX_Keys_Freechain_init((Freechain_Control *)&_POSIX_Keys_Keypool);
}

/**
 * @brief This routine is user defined freechain extension handle
 */
bool _POSIX_Keys_Freechain_extend(Freechain_Control *freechain)
{
  POSIX_Keys_Freechain *psx_freechain_p = (POSIX_Keys_Freechain *)freechain;
  size_t node_size = sizeof(POSIX_Keys_Key_value_pair);
  size_t size = psx_freechain_p->bump_count * node_size;
  int i;
  POSIX_Keys_Key_value_pair *nodes = _Workspace_Allocate(size);

  if (!nodes)
    return false;

  for ( i = 0; i < psx_freechain_p->bump_count; i++ ) {
      _Freechain_Put(freechain,
                     nodes + i);
  }
  return true;
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

  _POSIX_Keys_Keypool_init();
}
