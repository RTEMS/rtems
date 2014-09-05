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
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_KEY_H
#define _RTEMS_POSIX_KEY_H

#include <pthread.h>

#include <rtems/score/chain.h>
#include <rtems/score/object.h>
#include <rtems/score/rbtree.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIX_KEY POSIX Key
 *
 * @ingroup POSIXAPI
 *
 */
/**@{**/

/**
 * @brief Represents POSIX key and value pair.
 */
typedef struct {
  /**
   * @brief The chain node for the per-thread value chain.
   */
  Chain_Node Key_values_per_thread_node;

  /**
   * @brief The tree node for the lookup tree.
   */
  RBTree_Node Key_value_lookup_node;

  /**
   * @brief The POSIX key identifier used in combination with the thread
   * pointer as the tree key.
   */
  pthread_key_t key;

  /**
   * @brief The thread pointer used in combination with the POSIX key
   * identifier as the tree key.
   */
  Thread_Control *thread;

  /**
   * @brief The thread specific POSIX key value.
   */
  void *value;
} POSIX_Keys_Key_value_pair;

/**
 * @brief The data structure used to manage a POSIX key.
 */
typedef struct {
   /** This field is the Object control structure. */
   Objects_Control     Object;
   /** This field is the data destructor. */
   void (*destructor) (void *);
 }  POSIX_Keys_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
