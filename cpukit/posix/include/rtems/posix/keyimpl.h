/**
 * @file
 *
 * @brief Private Inlined Routines for POSIX Key's
 *
 * This include file contains the static inline implementation of the private
 * inlined routines for POSIX key's.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/posix/key.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/freechain.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/percpu.h>

#ifndef _RTEMS_POSIX_KEYIMPL_H
#define _RTEMS_POSIX_KEYIMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup POSIX_KEY
 *
 * @{
 */

/**
 * @brief The information control block used to manage this class of objects.
 */
POSIX_EXTERN Objects_Information  _POSIX_Keys_Information;

/**
 * @brief The rbtree control block used to manage all key values
 */
extern RBTree_Control _POSIX_Keys_Key_value_lookup_tree;

/**
 * @brief This freechain is used as a memory pool for POSIX_Keys_Key_value_pair.
 */
POSIX_EXTERN Freechain_Control _POSIX_Keys_Keypool;

#define POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( node ) \
  RTEMS_CONTAINER_OF( node, POSIX_Keys_Key_value_pair, Key_value_lookup_node )

/**
 * @brief POSIX key manager initialization.
 *
 * This routine performs the initialization necessary for this manager.
 */
void _POSIX_Key_Manager_initialization(void);

/**
 * @brief POSIX keys Red-Black tree node comparison.
 *
 * This routine compares the rbtree node
 */
RBTree_Compare_result _POSIX_Keys_Key_value_compare(
  const RBTree_Node *node1,
  const RBTree_Node *node2
);

/**
 * @brief Create thread-specific data POSIX key.
 *
 * This function executes all the destructors associated with the thread's
 * keys.  This function will execute until all values have been set to NULL.
 *
 * @param[in] thread is a pointer to the thread whose keys should have
 *            all their destructors run.
 *
 * NOTE: This is the routine executed when a thread exits to
 *       run through all the keys and do the destructor action.
 */
void _POSIX_Keys_Run_destructors(
  Thread_Control *thread
);

/**
 * @brief Free a POSIX key table memory.
 *
 * This memory frees the key table memory associated with @a the_key.
 *
 * @param[in] the_key is a pointer to the POSIX key to free
 * the table memory of.
 */
void _POSIX_Keys_Free_memory(
  POSIX_Keys_Control *the_key
);

/**
 * @brief Free a POSIX keys control block.
 *
 * This routine frees a keys control block to the
 * inactive chain of free keys control blocks.
 *
 * @param[in] the_key is a pointer to the POSIX key to free.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Keys_Free (
  POSIX_Keys_Control *the_key
);

/**
 * @brief Allocate a keys control block.
 *
 * This function allocates a keys control block from
 * the inactive chain of free keys control blocks.
 */

RTEMS_INLINE_ROUTINE POSIX_Keys_Control *_POSIX_Keys_Allocate( void )
{
  return (POSIX_Keys_Control *) _Objects_Allocate( &_POSIX_Keys_Information );
}

/**
 * @brief Free a keys control block.
 *
 * This routine frees a keys control block to the
 * inactive chain of free keys control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Keys_Free (
  POSIX_Keys_Control *the_key
)
{
  _Objects_Free( &_POSIX_Keys_Information, &the_key->Object );
}

/**
 * @brief Get a keys control block.
 *
 * This function maps key IDs to key control blocks.
 * If ID corresponds to a local keys, then it returns
 * the_key control pointer which maps to ID and location
 * is set to OBJECTS_LOCAL.  if the keys ID is global and
 * resides on a remote node, then location is set to OBJECTS_REMOTE,
 * and the_key is undefined.  Otherwise, location is set
 * to OBJECTS_ERROR and the_key is undefined.
 */

RTEMS_INLINE_ROUTINE POSIX_Keys_Control *_POSIX_Keys_Get (
  pthread_key_t      id,
  Objects_Locations *location
)
{
  return (POSIX_Keys_Control *)
    _Objects_Get( &_POSIX_Keys_Information, (Objects_Id) id, location );
}

POSIX_Keys_Key_value_pair * _POSIX_Keys_Key_value_pair_allocate( void );

RTEMS_INLINE_ROUTINE void _POSIX_Keys_Key_value_pair_free(
  POSIX_Keys_Key_value_pair *key_value_pair
)
{
  _Freechain_Put( &_POSIX_Keys_Keypool, key_value_pair );
}

RTEMS_INLINE_ROUTINE RBTree_Node *_POSIX_Keys_Find(
  pthread_key_t   key,
  Thread_Control *thread
)
{
  POSIX_Keys_Key_value_pair search_node;

  search_node.key = key;
  search_node.thread = thread;

  return _RBTree_Find(
    &_POSIX_Keys_Key_value_lookup_tree,
    &search_node.Key_value_lookup_node,
    _POSIX_Keys_Key_value_compare,
    true
  );
}

RTEMS_INLINE_ROUTINE void _POSIX_Keys_Free_key_value_pair(
  POSIX_Keys_Key_value_pair *key_value_pair
)
{
  _RBTree_Extract(
    &_POSIX_Keys_Key_value_lookup_tree,
    &key_value_pair->Key_value_lookup_node
  );
  _Chain_Extract_unprotected( &key_value_pair->Key_values_per_thread_node );
  _POSIX_Keys_Key_value_pair_free( key_value_pair );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
