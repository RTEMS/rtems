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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <termios.h>

#include <bsp/irq.h>

#include <rtems/termiostypes.h>
#include <libchip/serial.h>
#include <rtems/mouse_parser.h>
#include "keyboard.h"
#include "../../../shared/console_private.h"

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
  rtems_libio_ioctl_args_t *args = arg;

  switch (args->command) {
    default:
      if( vt_ioctl( args->command, (unsigned long)args->buffer ) != 0 )
        return rtems_termios_ioctl (arg);
      break;

    case MW_UID_REGISTER_DEVICE:
      printk( "SerialMouse: reg=%s\n", args->buffer );
      register_kbd_msg_queue( args->buffer, 0 );
      break;

    case MW_UID_UNREGISTER_DEVICE:
      unregister_kbd_msg_queue( 0 );
      break;
  }
 
  args->ioctl_return = 0;
  return RTEMS_SUCCESSFUL;
}
