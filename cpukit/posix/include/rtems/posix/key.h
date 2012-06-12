/**
 * @file rtems/posix/key.h
 *
 * This include file contains all the private support information for
 * POSIX key.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_KEY_H
#define _RTEMS_POSIX_KEY_H

#include <rtems/score/object.h>
#include <rtems/score/rbtree.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  This is the data Structure used to manage a POSIX key.
 *
 *  @note
 */
typedef struct {
   /** This field is the Object control structure. */
   Objects_Control     Object;
   void (*destructor) (void *);
 }  POSIX_Keys_Control;

/**
 *  This is the data Structure used to manage a POSIX key and value
 *  in the rbtree structure
 *  
 */
typdef struct {
  /** This field is the rbtree node structure. */
  RBTree_Node Node;
  /** This field is the POSIX key used as an rbtree key */
  pthread_key_t Key;
  /** This field is the Thread id also used as an rbtree key */
  Object_Id Thread_id;
  /** This field points to the POSIX key value of specific thread */
  void *Value;
 }  POSIX_Keys_Rbtree_node;
  
/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
POSIX_EXTERN Objects_Information  _POSIX_Keys_Information;

/**
 * The following defines the rbtree control block used to manage
 * all key value
 * 
 * problem: is it a proper way to define a global variable? what's the
 * exactly function of POSIX_EXTERN?
 */
POSIX_EXTERN RBTree_Control _POSIX_Keys_Rbtree;

/**
 *  @brief _POSIX_Keys_Manager_initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _POSIX_Keys_Manager_initialization(void);

/**
 * @brief _POSIX_Key_Rbtree_Compare_Function
 *
 * This routine compares the rbtree node
 */

int _POSIX_Keys_Rbtree_compare_function(
  const POSIX_Keys_Rbtree_node *node1,
  const POSIX_Keys_Rbtree_node *node2
);

/**
 *  @brief _POSIX_Keys_Run_destructors
 *
 *  This function executes all the destructors associated with the thread's
 *  keys.  This function will execute until all values have been set to NULL.
 *
 *  @param[in] thread is the thread whose keys should have all their
 *             destructors run.
 *
 *  @note This is the routine executed when a thread exits to
 *        run through all the keys and do the destructor action.
 */
void _POSIX_Keys_Run_destructors(
  Thread_Control *thread
);

/**
 *  @brief Free Key Memory
 *
 *  This memory frees the key table memory associated with @a the_key.
 *
 *  @param[in] the_key is the POSIX key to free the table memory of.
 */
void _POSIX_Keys_Free_memory(
  POSIX_Keys_Control *the_key
);

/**
 *  @brief _POSIX_Keys_Free
 *
 *  This routine frees a keys control block to the
 *  inactive chain of free keys control blocks.
 *
 *  @param[in] the_key is the POSIX key to free.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Keys_Free (
  POSIX_Keys_Control *the_key
);

#include <rtems/posix/key.inl>

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
