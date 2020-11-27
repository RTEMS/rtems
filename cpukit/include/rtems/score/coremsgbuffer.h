/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreMessageQueue
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreMessageQueue which are used by the implementation, the
 *   @ref RTEMSImplApplConfig, and the API.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1989, 2009 On-Line Applications Research Corporation (OAR)
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

#ifndef _RTEMS_SCORE_COREMSGBUFFER_H
#define _RTEMS_SCORE_COREMSGBUFFER_H

#include <rtems/score/basedefs.h>
#include <rtems/score/chain.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreMessageQueue
 *
 * @{
 */

/**
 * This define enables the support for priority based enqueuing of messages in
 * the Message Queue Handler.
 */
#define RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY

/**
 * @brief The structure is used to organize message buffers of a message queue.
 */
typedef struct {
  /**
   * @brief This member is used to enqueue the buffer in the pending or free
   *   buffer queue of a message queue.
   */
  Chain_Node Node;

  /** @brief This member defines the size of this message. */
  size_t size;

#if defined(RTEMS_SCORE_COREMSG_ENABLE_MESSAGE_PRIORITY)
  /** @brief This member defines the priority of this message. */
  int priority;
#endif

  /**
   * @brief This member contains the actual message.
   *
   * This is a zero-length array since the maximum message size is defined by
   * the user.  Use a size_t array to make sure that the member offset is at
   * the structure end.  This enables a more efficient memcpy() on 64-bit
   * targets and makes it easier to inspect the message buffers with a
   * debugger.
   */
  size_t buffer[ RTEMS_ZERO_LENGTH_ARRAY ];
} CORE_message_queue_Buffer;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_COREMSGBUFFER_H */
