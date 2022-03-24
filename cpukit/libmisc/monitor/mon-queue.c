/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Monitor Support for Message Queues
 */

/*
 * COPYRIGHT (c) 1989-2022. On-Line Applications Research Corporation (OAR).
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/monitor.h>
#include <rtems/rtems/messageimpl.h>

#include <stdio.h>

void
rtems_monitor_queue_canonical(
    rtems_monitor_queue_t *canonical_queue,
    const void            *queue_void
)
{
    const Message_queue_Control *rtems_queue = (const Message_queue_Control *) queue_void;

    canonical_queue->attributes = 0;

    if (
      rtems_queue->message_queue.operations
        == &_Thread_queue_Operations_priority
    ) {
      canonical_queue->attributes |= RTEMS_PRIORITY;
    }

#if defined(RTEMS_MULTIPROCESSING)
    if ( rtems_queue->is_global ) {
      canonical_queue->attributes |= RTEMS_GLOBAL;
    }
#endif

    canonical_queue->maximum_message_size = rtems_queue->message_queue.maximum_message_size;
    canonical_queue->maximum_pending_messages = rtems_queue->message_queue.maximum_pending_messages;
    canonical_queue->number_of_pending_messages = rtems_queue->message_queue.number_of_pending_messages;
}

void
rtems_monitor_queue_dump_header(
    bool verbose RTEMS_UNUSED
)
{
    fprintf(stdout,"\
  ID       NAME   ATTRIBUTES   PEND   MAXPEND  MAXSIZE\n");
/*23456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
0         1         2         3         4         5         6         7       */
    rtems_monitor_separator();
}


/*
 * Dump out the "next" queue indicated by 'id'.
 * Returns next one to check.
 * Returns RTEMS_OBJECT_ID_FINAL when all done
 */

void
rtems_monitor_queue_dump(
    rtems_monitor_queue_t *monitor_queue,
    bool  verbose RTEMS_UNUSED
)
{
    uint32_t              length = 0;

    length += rtems_monitor_dump_id(monitor_queue->id);
    length += rtems_monitor_pad(11, length);
    length += rtems_monitor_dump_name(monitor_queue->id);
    length += rtems_monitor_pad(19, length);
    length += rtems_monitor_dump_attributes(monitor_queue->attributes);
    length += rtems_monitor_pad(31, length);
    length += rtems_monitor_dump_decimal(monitor_queue->number_of_pending_messages);
    length += rtems_monitor_pad(39, length);
    length += rtems_monitor_dump_decimal(monitor_queue->maximum_pending_messages);
    length += rtems_monitor_pad(48, length);
    length += rtems_monitor_dump_decimal(monitor_queue->maximum_message_size);

    fprintf(stdout,"\n");
}
