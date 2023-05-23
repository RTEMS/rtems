/**
 * @file
 *
 * @ingroup TermiostypesSupport
 *
 * @brief This source file contains the implementation of
 *   rtems_termios_device_lock_acquire_default() and
 *   rtems_termios_device_lock_release_default().
 */

/*
 * Copyright (c) 1997 Eric Norum <eric@norum.ca>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/termiosdevice.h>

void
rtems_termios_device_lock_acquire_default(
  rtems_termios_device_context *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  rtems_interrupt_lock_acquire (&ctx->lock.interrupt, lock_context);
}

void
rtems_termios_device_lock_release_default(
  rtems_termios_device_context *ctx,
  rtems_interrupt_lock_context *lock_context
)
{
  rtems_interrupt_lock_release (&ctx->lock.interrupt, lock_context);
}
