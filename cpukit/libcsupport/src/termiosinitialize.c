/**
 *  @file
 *
 *  @brief Termios Initialization
 *  @ingroup Termios
 */

/*
 *  Author:
 *    W. Eric Norum
 *    Saskatchewan Accelerator Laboratory
 *    University of Saskatchewan
 *    Saskatoon, Saskatchewan, CANADA
 *    eric@skatter.usask.ca
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/termiostypes.h>

rtems_mutex rtems_termios_ttyMutex = RTEMS_MUTEX_INITIALIZER( "termios" );

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
