/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018, 2019 embedded brains GmbH
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

/*
 * This file must be compatible to general purpose POSIX system, e.g. Linux,
 * FreeBSD.  It may be used for utility programs.
 */

#ifndef _RTEMS_RECORDDATA_H
#define _RTEMS_RECORDDATA_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSRecord Event Recording
 *
 * @brief Low-level event recording support.
 *
 * @{
 */

/**
 * @brief The record version.
 *
 * The record version reflects the record event definitions.  It is reported by
 * the RTEMS_RECORD_VERSION event.
 */
#define RTEMS_RECORD_THE_VERSION 1

/**
 * @brief The items are in 32-bit little-endian format.
 */
#define RTEMS_RECORD_FORMAT_LE_32 0x11111111

/**
 * @brief The items are in 64-bit little-endian format.
 */
#define RTEMS_RECORD_FORMAT_LE_64 0x22222222

/**
 * @brief The items are in 32-bit big-endian format.
 */
#define RTEMS_RECORD_FORMAT_BE_32 0x33333333

/**
 * @brief The items are in 64-bit big-endian format.
 */
#define RTEMS_RECORD_FORMAT_BE_64 0x44444444

/**
 * @brief Magic number to identify a record item stream.
 *
 * This is a random number.
 */
#define RTEMS_RECORD_MAGIC 0x82e14ec1

/**
 * @brief The record events.
 */
typedef enum {
  /* There are 512 events reserved for the system */
  RTEMS_RECORD_EMPTY,
  RTEMS_RECORD_VERSION,

  /*
   * Keep the following system events in lexicographical order, increment
   * RTEMS_RECORD_THE_VERSION after each change.
   */
  RTEMS_RECORD_ACCEPT,
  RTEMS_RECORD_BIND,
  RTEMS_RECORD_BUFFER,
  RTEMS_RECORD_CHOWN,
  RTEMS_RECORD_CLOSE,
  RTEMS_RECORD_CONNECT,
  RTEMS_RECORD_COUNT,
  RTEMS_RECORD_ETHER_INPUT,
  RTEMS_RECORD_ETHER_OUTPUT,
  RTEMS_RECORD_FCHMOD,
  RTEMS_RECORD_FCNTL,
  RTEMS_RECORD_FDATASYNC,
  RTEMS_RECORD_FREQUENCY,
  RTEMS_RECORD_FSTAT,
  RTEMS_RECORD_FSYNC,
  RTEMS_RECORD_FTRUNCATE,
  RTEMS_RECORD_GIT_HASH,
  RTEMS_RECORD_HEAD,
  RTEMS_RECORD_HEAP_ALLOC,
  RTEMS_RECORD_HEAP_FREE,
  RTEMS_RECORD_HEAP_SIZE,
  RTEMS_RECORD_HEAP_USAGE,
  RTEMS_RECORD_INTERUPT_BEGIN,
  RTEMS_RECORD_INTERUPT_END,
  RTEMS_RECORD_INTERUPT_INSTALL,
  RTEMS_RECORD_INTERUPT_REMOVE,
  RTEMS_RECORD_IOCTL,
  RTEMS_RECORD_IP6_INPUT,
  RTEMS_RECORD_IP6_OUTPUT,
  RTEMS_RECORD_IP_INPUT,
  RTEMS_RECORD_IP_OUTPUT,
  RTEMS_RECORD_KEVENT,
  RTEMS_RECORD_KQUEUE,
  RTEMS_RECORD_LENGTH,
  RTEMS_RECORD_LINK,
  RTEMS_RECORD_LSEEK,
  RTEMS_RECORD_MKNOD,
  RTEMS_RECORD_MMAP,
  RTEMS_RECORD_MOUNT,
  RTEMS_RECORD_OPEN,
  RTEMS_RECORD_OVERFLOW,
  RTEMS_RECORD_PAGE_ALLOC,
  RTEMS_RECORD_PAGE_FREE,
  RTEMS_RECORD_POLL,
  RTEMS_RECORD_PROCESSOR,
  RTEMS_RECORD_PROCESSOR_MAXIMUM,
  RTEMS_RECORD_READ,
  RTEMS_RECORD_READLINK,
  RTEMS_RECORD_READV,
  RTEMS_RECORD_RECV,
  RTEMS_RECORD_RECVFROM,
  RTEMS_RECORD_RECVMSG,
  RTEMS_RECORD_RENAME,
  RTEMS_RECORD_RTEMS_BARRIER_CREATE,
  RTEMS_RECORD_RTEMS_BARRIER_DELETE,
  RTEMS_RECORD_RTEMS_BARRIER_RELEASE,
  RTEMS_RECORD_RTEMS_BARRIER_WAIT,
  RTEMS_RECORD_RTEMS_EVENT_RECEIVE,
  RTEMS_RECORD_RTEMS_EVENT_SEND,
  RTEMS_RECORD_RTEMS_EVENT_SYSTEM_RECEIVE,
  RTEMS_RECORD_RTEMS_EVENT_SYSTEM_SEND,
  RTEMS_RECORD_RTEMS_MESSAGE_QUEUE_BROADCAST,
  RTEMS_RECORD_RTEMS_MESSAGE_QUEUE_CREATE,
  RTEMS_RECORD_RTEMS_MESSAGE_QUEUE_DELETE,
  RTEMS_RECORD_RTEMS_MESSAGE_QUEUE_FLUSH,
  RTEMS_RECORD_RTEMS_MESSAGE_QUEUE_RECEIVE,
  RTEMS_RECORD_RTEMS_MESSAGE_QUEUE_SEND,
  RTEMS_RECORD_RTEMS_MESSAGE_QUEUE_URGENT,
  RTEMS_RECORD_RTEMS_PARTITION_CREATE,
  RTEMS_RECORD_RTEMS_PARTITION_DELETE,
  RTEMS_RECORD_RTEMS_PARTITION_GET_BUFFER,
  RTEMS_RECORD_RTEMS_PARTITION_RETURN_BUFFER,
  RTEMS_RECORD_RTEMS_RATE_MONOTONIC_CANCEL,
  RTEMS_RECORD_RTEMS_RATE_MONOTONIC_CREATE,
  RTEMS_RECORD_RTEMS_RATE_MONOTONIC_DELETE,
  RTEMS_RECORD_RTEMS_RATE_MONOTONIC_PERIOD,
  RTEMS_RECORD_RTEMS_SEMAPHORE_CREATE,
  RTEMS_RECORD_RTEMS_SEMAPHORE_DELETE,
  RTEMS_RECORD_RTEMS_SEMAPHORE_FLUSH,
  RTEMS_RECORD_RTEMS_SEMAPHORE_OBTAIN,
  RTEMS_RECORD_RTEMS_SEMAPHORE_RELEASE,
  RTEMS_RECORD_RTEMS_TIMER_CANCEL,
  RTEMS_RECORD_RTEMS_TIMER_CREATE,
  RTEMS_RECORD_RTEMS_TIMER_DELETE,
  RTEMS_RECORD_RTEMS_TIMER_FIRE_AFTER,
  RTEMS_RECORD_RTEMS_TIMER_FIRE_WHEN,
  RTEMS_RECORD_RTEMS_TIMER_RESET,
  RTEMS_RECORD_RTEMS_TIMER_SERVER_FIRE_AFTER,
  RTEMS_RECORD_RTEMS_TIMER_SERVER_FIRE_WHEN,
  RTEMS_RECORD_SELECT,
  RTEMS_RECORD_SEND,
  RTEMS_RECORD_SENDMSG,
  RTEMS_RECORD_SENDTO,
  RTEMS_RECORD_SOCKET,
  RTEMS_RECORD_STATVFS,
  RTEMS_RECORD_SYMLINK,
  RTEMS_RECORD_TAIL,
  RTEMS_RECORD_TCP_INPUT,
  RTEMS_RECORD_TCP_OUTPUT,
  RTEMS_RECORD_THREAD_BEGIN,
  RTEMS_RECORD_THREAD_CREATE,
  RTEMS_RECORD_THREAD_DELETE,
  RTEMS_RECORD_THREAD_EXIT,
  RTEMS_RECORD_THREAD_EXITTED,
  RTEMS_RECORD_THREAD_ID,
  RTEMS_RECORD_THREAD_PRIO_CURRENT_HIGH,
  RTEMS_RECORD_THREAD_PRIO_CURRENT_LOW,
  RTEMS_RECORD_THREAD_PRIO_REAL_HIGH,
  RTEMS_RECORD_THREAD_PRIO_REAL_LOW,
  RTEMS_RECORD_THREAD_QUEUE_ENQUEUE,
  RTEMS_RECORD_THREAD_QUEUE_ENQUEUE_STICKY,
  RTEMS_RECORD_THREAD_QUEUE_EXTRACT,
  RTEMS_RECORD_THREAD_QUEUE_SURRENDER,
  RTEMS_RECORD_THREAD_QUEUE_SURRENDER_STICKY,
  RTEMS_RECORD_THREAD_RESTART,
  RTEMS_RECORD_THREAD_STACK_CURRENT,
  RTEMS_RECORD_THREAD_STACK_SIZE,
  RTEMS_RECORD_THREAD_STACK_USAGE,
  RTEMS_RECORD_THREAD_START,
  RTEMS_RECORD_THREAD_STATE_CLEAR,
  RTEMS_RECORD_THREAD_STATE_SET,
  RTEMS_RECORD_THREAD_SWITCH_IN,
  RTEMS_RECORD_THREAD_SWITCH_OUT,
  RTEMS_RECORD_THREAD_TERMINATE,
  RTEMS_RECORD_UDP_INPUT,
  RTEMS_RECORD_UDP_OUTPUT,
  RTEMS_RECORD_UMA_ALLOC_PTR,
  RTEMS_RECORD_UMA_ALLOC_ZONE,
  RTEMS_RECORD_UMA_FREE_PTR,
  RTEMS_RECORD_UMA_FREE_ZONE,
  RTEMS_RECORD_UNLINK,
  RTEMS_RECORD_UNMOUNT,
  RTEMS_RECORD_UPTIME_HIGH,
  RTEMS_RECORD_UPTIME_LOW,
  RTEMS_RECORD_WORKSPACE_ALLOC,
  RTEMS_RECORD_WORKSPACE_FREE,
  RTEMS_RECORD_WORKSPACE_SIZE,
  RTEMS_RECORD_WORKSPACE_USAGE,
  RTEMS_RECORD_WRITE,
  RTEMS_RECORD_WRITEV,

  /* There are 512 events reserved for the user */
  RTEMS_RECORD_USER = 512,

  RTEMS_RECORD_LAST = 1023
} rtems_record_event;

/**
 * @brief Bits in the record item event member reserved for the actual event.
 */
#define RTEMS_RECORD_EVENT_BITS 10

/**
 * @brief Bits in the record item event member reserved for the time of the
 * event.
 */
#define RTEMS_RECORD_TIME_BITS 22

/**
 * @brief Builds a time event for the specified time stamp and event.
 *
 * The events are stored in the record item with a time stamp.  There are 22
 * bits allocated to the time stamp and 10 bits allocated to the event.  The 22
 * bits are enough to get reliable time stamps on a system with a 4GHz CPU
 * counter and a 1000Hz clock tick.
 */
#define RTEMS_RECORD_TIME_EVENT( time, event ) \
  ( ( ( time ) << RTEMS_RECORD_EVENT_BITS ) | ( event ) )

/**
 * @brief Gets the time of a time event.
 */
#define RTEMS_RECORD_GET_TIME( time_event ) \
  ( ( time_event ) >> RTEMS_RECORD_EVENT_BITS )

/**
 * @brief Gets the event of a time event.
 */
#define RTEMS_RECORD_GET_EVENT( time_event ) \
  ( ( time_event ) & ( ( 1U << RTEMS_RECORD_EVENT_BITS ) - 1U ) )

/**
 * @brief The record data integer type.
 *
 * It is big enough to store 32-bit integers and pointers.
 */
typedef unsigned long rtems_record_data;

/**
 * @brief The native record item.
 */
typedef struct __attribute__((__packed__)) {
  uint32_t          event;
  rtems_record_data data;
} rtems_record_item;

/**
 * @brief The 32-bit format record item.
 */
typedef struct {
  uint32_t event;
  uint32_t data;
} rtems_record_item_32;

/**
 * @brief The 64-bit format record item.
 */
typedef struct __attribute__((__packed__)) {
  uint32_t event;
  uint64_t data;
} rtems_record_item_64;

const char *rtems_record_event_text( rtems_record_event event );

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_RECORDDATA_H */
