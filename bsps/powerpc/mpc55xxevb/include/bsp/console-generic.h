/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Generic console driver API.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_SHARED_CONSOLE_GENERIC_H
#define LIBBSP_SHARED_CONSOLE_GENERIC_H

#include <rtems/libio.h>
#include <rtems/termiostypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  rtems_termios_callbacks termios_callbacks;
  int (*poll_read)(int minor);
  void (*poll_write)(int minor, char c);
} console_generic_callbacks;

typedef struct {
  void *context;
  const console_generic_callbacks *callbacks;
  const char *device_path;
} console_generic_info;

extern const console_generic_info console_generic_info_table [];

extern const size_t console_generic_info_count;

extern const rtems_device_minor_number console_generic_minor;

#define CONSOLE_GENERIC_INFO_TABLE \
  const console_generic_info console_generic_info_table []

#define CONSOLE_GENERIC_INFO(context, callbacks, device_path) \
  { context, callbacks, device_path }

#define CONSOLE_GENERIC_INFO_COUNT \
  const size_t console_generic_info_count = \
    sizeof(console_generic_info_table) / sizeof(console_generic_info_table [0])

#define CONSOLE_GENERIC_MINOR(minor) \
  const rtems_device_minor_number console_generic_minor = (minor)

static inline void *console_generic_get_context(int minor)
{
  return console_generic_info_table [minor].context;
}

static inline struct rtems_termios_tty *console_generic_get_tty_at_open(
  void *arg
)
{
  const rtems_libio_open_close_args_t *oc =
    (const rtems_libio_open_close_args_t *) arg;

  return (struct rtems_termios_tty *) oc->iop->data1;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_SHARED_CONSOLE_GENERIC_H */
