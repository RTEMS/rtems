/**
 * @file
 *
 * @brief POSIX Key Private Support
 *
 * This include file contains all the private support information for
 * POSIX key.
 */

/*
 * Copyright (c) 2012 Zhongwei Yao.
 * COPYRIGHT (c) 1989-2011.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_KEY_H
#define _RTEMS_POSIX_KEY_H

#include <rtems/score/rbtree.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/freechain.h>
#include <rtems/score/objectimpl.h>

/**
 * @defgroup POSIX_KEY POSIX Key
 *
 * @ingroup POSIXAPI
 *
 */
/**@{**/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The rbtree node used to manage a POSIX key and value.
 */
typedef struct {
  /** This field is the chain node structure. */
  Chain_Node Key_values_per_thread_node;
  /** This field is the rbtree node structure. */
  RBTree_Node Key_value_lookup_node;
  /** This field is the POSIX key used as an rbtree key */
  pthread_key_t key;
  /** This field is the Thread id also used as an rbtree key */
  Objects_Id thread_id;
  /** This field points to the POSIX key value of specific thread */
  void *value;
}  POSIX_Keys_Key_value_pair;

/**
 * @brief POSIX_Keys_Freechain is used in Freechain structure
 */
typedef struct {
    Freechain_Control super_fc;
    size_t bump_count;
} POSIX_Keys_Freechain;

/**
 * @brief The data structure used to manage a POSIX key.
 */
typedef struct {
   /** This field is the Object control structure. */
   Objects_Control     Object;
   /** This field is the data destructor. */
   void (*destructor) (void *);
 }  POSIX_Keys_Control;

/**
 * @brief The information control block used to manage this class of objects.
 */
POSIX_EXTERN Objects_Information  _POSIX_Keys_Information;

/**
 * @brief The rbtree control block used to manage all key values
 */
POSIX_EXTERN RBTree_Control _POSIX_Keys_Key_value_lookup_tree;

/**
 * @brief This freechain is used as a memory pool for POSIX_Keys_Key_value_pair.
 */
POSIX_EXTERN POSIX_Keys_Freechain _POSIX_Keys_Keypool;

/**
 * @brief POSIX key manager initialization.
 *
 * This routine performs the initialization necessary for this manager.
 */
void _POSIX_Key_Manager_initialization(void);

/**
 * @brief POSIX key Freechain extend handle
 *
 * This routine extend freechain node, which is called in freechain_get
 * automatically.
 */
bool _POSIX_Keys_Freechain_extend(Freechain_Control *freechain);

/**
 * @brief POSIX keys Red-Black tree node comparison.
 *
 * This routine compares the rbtree node
 */
int _POSIX_Keys_Key_value_lookup_tree_compare_function(
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

#include <rtems/posix/key.inl>

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
