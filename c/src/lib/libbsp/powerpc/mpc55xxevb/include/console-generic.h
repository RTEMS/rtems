/**
 * @file
 *
 * @brief Generic console driver API.
 */

/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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
