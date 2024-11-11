/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 2018, 2024 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_RECORDCLIENT_H
#define _RTEMS_RECORDCLIENT_H

#include "recorddata.h"

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup RTEMSRecord
 *
 * @{
 */

#define RTEMS_RECORD_CLIENT_MAXIMUM_CPU_COUNT 32

typedef enum {
  RTEMS_RECORD_CLIENT_SUCCESS,
  RTEMS_RECORD_CLIENT_ERROR_INVALID_MAGIC,
  RTEMS_RECORD_CLIENT_ERROR_UNKNOWN_FORMAT,
  RTEMS_RECORD_CLIENT_ERROR_UNSUPPORTED_VERSION,
  RTEMS_RECORD_CLIENT_ERROR_UNSUPPORTED_CPU,
  RTEMS_RECORD_CLIENT_ERROR_UNSUPPORTED_CPU_MAX,
  RTEMS_RECORD_CLIENT_ERROR_DOUBLE_CPU_MAX,
  RTEMS_RECORD_CLIENT_ERROR_DOUBLE_PER_CPU_COUNT,
  RTEMS_RECORD_CLIENT_ERROR_NO_CPU_MAX,
  RTEMS_RECORD_CLIENT_ERROR_NO_MEMORY,
  RTEMS_RECORD_CLIENT_ERROR_PER_CPU_ITEMS_OVERFLOW
} rtems_record_client_status;

typedef rtems_record_client_status ( *rtems_record_client_handler )(
  uint64_t            bt,
  uint32_t            cpu,
  rtems_record_event  event,
  uint64_t            data,
  void               *arg
);

typedef struct {
  uint64_t uptime_bt;
  uint32_t time_last;
  uint64_t time_accumulated;
} rtems_record_client_uptime;

/**
 * @brief This constant defines the maximum capacity of the hold back item
 *   storage in case a reallocation is necessary.
 */
#define RTEMS_RECORD_CLIENT_HOLD_BACK_REALLOCATION_LIMIT 0x100000

typedef struct {
  /**
   * @brief Event time to uptime maintenance.
   */
  rtems_record_client_uptime uptime;

  /**
   * @brief The binary time of the last item.
   */
  uint64_t last_bt;

  /**
   * @brief Last RTEMS_RECORD_UPTIME_LOW data.
   */
  uint32_t uptime_low;

  /**
   * @brief If true, then uptime low value is valid.
   */
  bool uptime_low_valid;

  /**
   * @brief If true, then hold back items.
   */
  bool hold_back;

  /**
   * @brief Storage for hold back items.
   *
   * Once the time stamp association with the uptime is known, the hold back
   * items can be processed.
   */
  rtems_record_item_64 *items;

  /**
   * @brief The item capacity of the hold back storage.
   */
  size_t item_capacity;

  /**
   * @brief The index for the next hold back item.
   */
  size_t item_index;
} rtems_record_client_per_cpu;

typedef struct rtems_record_client_context {
  uint64_t to_bt_scaler;
  rtems_record_client_per_cpu per_cpu[ RTEMS_RECORD_CLIENT_MAXIMUM_CPU_COUNT ];
  uint32_t cpu;
  uint32_t cpu_count;
  uint32_t per_cpu_items;
  union {
    rtems_record_item_32 format_32;
    rtems_record_item_64 format_64;
  } item;
  size_t todo;
  void *pos;
  rtems_record_client_status ( *consume )(
    struct rtems_record_client_context *,
    const void *,
    size_t
  );
  rtems_record_client_handler handler;
  void *handler_arg;
  size_t data_size;
  uint32_t header[ 2 ];
  rtems_record_client_status status;
} rtems_record_client_context;

/**
 * @brief Initializes a record client.
 *
 * The record client consumes a record item stream produces by the record
 * server.
 *
 * @param ctx The record client context to initialize.
 * @param handler The handler is invoked for each received record item.
 * @param arg The handler argument.
 */
rtems_record_client_status rtems_record_client_init(
  rtems_record_client_context *ctx,
  rtems_record_client_handler  handler,
  void                        *arg
);

/**
 * @brief Runs the record client to consume new stream data.
 *
 * @param ctx The record client context.
 * @param buf The buffer with new stream data.
 * @param n The size of the buffer.
 */
rtems_record_client_status rtems_record_client_run(
  rtems_record_client_context *ctx,
  const void                  *buf,
  size_t                       n
);

/**
 * @brief Drains all internal buffers and frees the allocated resources.
 *
 * The client context must not be used afterwards.  It can be re-initialized
 * via rtems_record_client_init().
 *
 * @param ctx The record client context.
 */
void rtems_record_client_destroy(
  rtems_record_client_context *ctx
);

static inline void rtems_record_client_set_handler(
  rtems_record_client_context *ctx,
  rtems_record_client_handler  handler
)
{
  ctx->handler = handler;
}

static inline uint64_t rtems_record_client_bintime_to_nanoseconds(
  uint64_t bt
)
{
  uint64_t ns_per_sec;
  uint64_t nanoseconds;

  ns_per_sec = 1000000000ULL;
  nanoseconds = ns_per_sec * ( (uint32_t) ( bt >> 32 ) );
  nanoseconds += ( ns_per_sec * (uint32_t) bt ) >> 32;

  return nanoseconds;
}

static inline void rtems_record_client_bintime_to_seconds_and_nanoseconds(
  uint64_t  bt,
  uint32_t *seconds,
  uint32_t *nanoseconds
)
{
  uint64_t ns_per_sec;

  ns_per_sec = 1000000000ULL;
  *seconds = (uint32_t) ( bt >> 32 );
  *nanoseconds = (uint32_t) ( ( ns_per_sec * (uint32_t) bt ) >> 32 );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_RECORDCLIENT_H */
