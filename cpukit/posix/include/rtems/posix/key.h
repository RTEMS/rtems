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

#include <pthread.h>

#include <rtems/score/chain.h>
#include <rtems/score/object.h>
#include <rtems/score/rbtree.h>

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
