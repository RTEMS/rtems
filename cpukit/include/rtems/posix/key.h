/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
   * @brief The chain node for the key value pairs chain in POSIX_Keys_Control.
   */
  Chain_Node Key_node;

  /**
   * @brief The tree node for the lookup tree in Thread_Keys_information.
   */
  RBTree_Node Lookup_node;

  /**
   * @brief The POSIX key identifier used as the tree key.
   */
  pthread_key_t key;

  /**
   * @brief The corresponding thread.
   */
  Thread_Control *thread;

  /**
   * @brief The thread specific POSIX key value.
   */
  void *value;
} POSIX_Keys_Key_value_pair;

/**
 * @brief The initial set of POSIX key and value pairs.
 *
 * This array is provided via <rtems/confdefs.h> in case POSIX key and value
 * pairs are configured.  The POSIX key and value pair count in this array must
 * be equal to
 * _Objects_Maximum_per_allocation( _POSIX_Keys_Key_value_pair_maximum ).
 */
extern POSIX_Keys_Key_value_pair _POSIX_Keys_Key_value_pairs[];

/**
 * @brief The POSIX key and value pairs maximum.
 *
 * This value is provided via <rtems/confdefs.h> in case POSIX key and value
 * pairs are configured.  The OBJECTS_UNLIMITED_OBJECTS flag may be set.
 */
extern const uint32_t _POSIX_Keys_Key_value_pair_maximum;

/**
 * @brief The data structure used to manage a POSIX key.
 */
typedef struct {
   /** This field is the Object control structure. */
   Objects_Control     Object;
   /** This field is the data destructor. */
   void (*destructor) (void *);

   /**
    * @brief Key value pairs of this key.
    */
   Chain_Control Key_value_pairs;
 }  POSIX_Keys_Control;

/**
 * @brief The POSIX Key objects information.
 */
extern Objects_Information _POSIX_Keys_Information;

/**
 * @brief Macro to define the objects information for the POSIX Key objects.
 *
 * This macro should only be used by <rtems/confdefs.h>.
 *
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 * may be set).
 */
#define POSIX_KEYS_INFORMATION_DEFINE( max ) \
  OBJECTS_INFORMATION_DEFINE( \
    _POSIX_Keys, \
    OBJECTS_POSIX_API, \
    OBJECTS_POSIX_KEYS, \
    POSIX_Keys_Control, \
    max, \
    OBJECTS_NO_STRING_NAME, \
    NULL \
  )

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
