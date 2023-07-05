/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RegulatorInternalAPI
 *
 * @brief Regulator Library Implementation Support
 */

/*
 * Copyright (C) 2023 On-Line Applications Research Corporation (OAR)
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

/**
 * @defgroup RegulatorInternalAPI Regulator API Internals
 *
 * @brief Regulator Internal Information
 *
 * This concerns implementation information about the Regulator.
 */

#ifndef RTEMS_REGULATORIMPL_H
#define RTEMS_REGULATORIMPL_H

#include <stdatomic.h>

#include <rtems/chain.h>


/**
 * @ingroup RegulatorInternalAPI
 *
 * This constant is used to indicate the regulator instance is initialized.
 */
#define REGULATOR_INITIALIZED 0xDeadF00d

/**
 * @ingroup RegulatorInternalAPI
 *
 * @brief Regulator Message Instance Management Structure
 */
typedef struct {
  /** This points to the message contents. */
  void   *buffer;
  /** This is the length of the message. */
  size_t  length;
} _Regulator_Message_t;

/**
 * @ingroup RegulatorInternalAPI
 *
 * @brief Regulator Statistics Private Structure
 *
 * An instance of this structure is allocated per regulator instance.
 */
typedef struct {
  /** Number of successfully obtained buffers. */
  atomic_size_t    obtained;
  
  /** Number of successfully released buffers. */
  atomic_size_t    released;

  /** Number of successfully delivered buffers. */
  atomic_size_t    delivered;
} _Regulator_Statistics;
 
/**
 * @ingroup RegulatorInternalAPI
 *
 * @brief Regulator Instance Private Structure
 *
 * An instance of this structure is allocated per regulator instance.
 */
typedef struct {
  /** Has magic value when instance is usable */
  uint32_t   initialized;

  /** Attributes for this instance -- copied from user */
  rtems_regulator_attributes  Attributes;

  /** Pointer to allocated message memory */
  void *message_memory;

  /** Pointer to allocated memory for RTEMS Message Queue for pending buffers*/
  void *message_queue_storage;

  /** RTEMS Message Queue of pending outgoing messages */
  rtems_id  queue_id;

  /** RTEMS Partition for pool of unused messages */
  rtems_id  messages_partition_id;

  /** RTEMS Task for performing output */
  rtems_id  delivery_thread_id;

  /** Id of period used by output thread */
  rtems_id  delivery_thread_period_id;

  /** Indicates Delivery thread is running */
  bool  delivery_thread_is_running;

  /** Indicates Delivery thread has been requested to exit */
  bool  delivery_thread_request_exit;

  /** Indicates Delivery thread has exited */
  bool  delivery_thread_has_exited;

  /** Internal Statistics */
  _Regulator_Statistics Statistics;

} _Regulator_Control;

#endif /* RTEMS_REGULATORIMPL_H */
