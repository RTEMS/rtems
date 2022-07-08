/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file is an extension of the generic console driver
 *  shell used by all console drivers using libchip, it contains
 *  the console_control routine,  This bsp needs its own version
 *  of this method to handle the keyboard and mouse as a single
 *  device.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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

#include <bsp.h>
#include <stdlib.h>
#include <assert.h>
#include <termios.h>
#include <rtems/libio.h>
#include <rtems/console.h>

#include <bsp/irq.h>

#include <rtems/termiostypes.h>
#include <libchip/serial.h>
#include <rtems/mouse_parser.h>
#if BSP_ENABLE_VGA
#include <rtems/keyboard.h>
#endif
#include "../../shared/dev/serial/legacy-console.h"

/*
 *  console_control
 *
 *  this routine uses the termios driver to process io
 */
rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
#if BSP_ENABLE_VGA
  if (minor == 0) {
    rtems_libio_ioctl_args_t *args = arg;

    switch (args->command) {
      default:
        if( vt_ioctl( args->command, (unsigned long)args->buffer ) != 0 )
          return rtems_termios_ioctl (arg);
        break;

      case MW_UID_REGISTER_DEVICE:
        printk( "SerialMouse: reg=%s\n", (const char*) args->buffer );
        register_kbd_msg_queue( args->buffer, 0 );
        break;

      case MW_UID_UNREGISTER_DEVICE:
        unregister_kbd_msg_queue( 0 );
        break;
    }

    args->ioctl_return = 0;
    return RTEMS_SUCCESSFUL;
  }
#endif
  return rtems_termios_ioctl (arg);
}
