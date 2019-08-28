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

#ifndef _RTEMS_RECORDSERVER_H
#define _RTEMS_RECORDSERVER_H

#include "recorddata.h"

#include <sys/types.h>
#include <rtems.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief Drains the record items on all processors an writes them to the file
 * descriptor.
 *
 * @param fd The file descriptor.
 * @param written Set to true if items were written to the file descriptor,
 *   otherwise set to false.
 *
 * @retval The bytes written to the file descriptor.
 */
ssize_t rtems_record_writev( int fd, bool *written );

/**
 * @brief Runs a record TCP server loop.
 *
 * @param port The TCP port to listen in host byte order.
 * @param period The drain period in clock ticks.
 */
void rtems_record_server( uint16_t port, rtems_interval period );

/**
 * @brief Starts a record TCP server task.
 *
 * @param priority The task priority.
 * @param port The TCP port to listen in host byte order.
 * @param period The drain period in clock ticks.
 */
rtems_status_code rtems_record_start_server(
  rtems_task_priority priority,
  uint16_t            port,
  rtems_interval      period
);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_RECORDSERVER_H */
