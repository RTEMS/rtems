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

#ifndef _RTEMS_RECORDCLIENT_H
#define _RTEMS_RECORDCLIENT_H

#include "recorddata.h"

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
  RTEMS_RECORD_CLIENT_ERROR_UNSUPPORTED_CPU
} rtems_record_client_status;

typedef rtems_record_client_status ( *rtems_record_client_handler )(
  uint32_t            seconds,
  uint32_t            nanoseconds,
  uint32_t            cpu,
  rtems_record_event  event,
  uint64_t            data,
  void               *arg
);

typedef struct {
  struct {
    uint64_t bt;
    uint32_t time_at_bt;
    uint32_t time_last;
    uint32_t time_accumulated;
  } uptime;
  uint32_t tail[ 2 ];
  uint32_t head[ 2 ];
  size_t index;
} rtems_record_client_per_cpu;

typedef struct rtems_record_client_context {
  uint64_t to_bt_scaler;
  rtems_record_client_per_cpu per_cpu[ RTEMS_RECORD_CLIENT_MAXIMUM_CPU_COUNT ];
  uint64_t data;
  uint32_t cpu;
  uint32_t event;
  uint32_t count;
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
  uint32_t header[ 2 ];
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
void rtems_record_client_init(
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

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_RECORDCLIENT_H */
