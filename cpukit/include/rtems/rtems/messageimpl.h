/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicMessage
 *
 * @brief This header file provides the implementation interfaces of
 *   the @ref RTEMSImplClassicMessage.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_RTEMS_MESSAGEIMPL_H
#define _RTEMS_RTEMS_MESSAGEIMPL_H

#include <rtems/rtems/messagedata.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/coremsgimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSImplClassicMessage Message Manager
 *
 * @ingroup RTEMSImplClassic
 *
 * @brief This group contains the Message Manager implementation.
 *
 * @{
 */

/**
 *  The following enumerated type details the modes in which a message
 *  may be submitted to a message queue.  The message may be posted
 *  in a send or urgent fashion.
 */
typedef enum {
  /**
   *  This value indicates the user wants to send the message using the
   *  normal message insertion protocol (FIFO or priority).
   */
  MESSAGE_QUEUE_SEND_REQUEST   = 0,
  /**
   *  This value indicates the user considers the message to be urgent
   *  and wants it inserted at the head of the pending message queue.
   */
  MESSAGE_QUEUE_URGENT_REQUEST = 1
}  Message_queue_Submit_types;

/**
 *  @brief Message_queue_Submit
 *
 *  This routine implements the directives rtems_message_queue_send
 *  and rtems_message_queue_urgent.  It processes a message that is
 *  to be submitted to the designated message queue.  The message will
 *  either be processed as a send send message which it will be inserted
 *  at the rear of the queue or it will be processed as an urgent message
 *  which will be inserted at the front of the queue.
 */
rtems_status_code _Message_queue_Submit(
  rtems_id                    id,
  const void                 *buffer,
  size_t                      size,
  Message_queue_Submit_types  submit_type
);

/**
 *  @brief Deallocates a message queue control block into
 *  the inactive chain of free message queue control blocks.
 *
 *  This routine deallocates a message queue control block into
 *  the inactive chain of free message queue control blocks.
 */
static inline void _Message_queue_Free (
  Message_queue_Control *the_message_queue
)
{
  _Objects_Free( &_Message_queue_Information, &the_message_queue->Object );
}

static inline Message_queue_Control *_Message_queue_Get(
  Objects_Id            id,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Context_initialize( queue_context );
  return (Message_queue_Control *) _Objects_Get(
    id,
    &queue_context->Lock_context.Lock_context,
    &_Message_queue_Information
  );
}

static inline Message_queue_Control *_Message_queue_Allocate( void )
{
  return (Message_queue_Control *)
    _Objects_Allocate( &_Message_queue_Information );
}

/**
 * @brief Creates a message queue.
 *
 * @param config is the message queue configuration.
 *
 * @param[out] id contains the object identifier if the operation was
 *   successful.
 *
 * @param allocate_buffers is the message buffer storage area allocation
 *   handler.
 */
rtems_status_code _Message_queue_Create(
  const rtems_message_queue_config    *config,
  rtems_id                            *id,
  CORE_message_queue_Allocate_buffers  allocate_buffers
);

/**@}*/

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/rtems/msgmp.h>
#endif

#endif
/* end of include file */
