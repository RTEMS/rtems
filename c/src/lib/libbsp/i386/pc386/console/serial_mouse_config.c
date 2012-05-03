/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <bsp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "libchip/serial.h"
#include "../../../shared/console_private.h"

/* select which serial port the mouse is connected to */
#if defined(SERIAL_MOUSE_COM2)
  #define MOUSE_DEVICE "/dev/com2"
#else
  #define MOUSE_DEVICE "/dev/com1"
#endif

static const char *SerialMouseDevice = MOUSE_DEVICE;

bool bsp_get_serial_mouse_device(
  const char **name,
  const char **type
)
{
  const char *consname;

  *name = SerialMouseDevice;
  *type = "ms";

  /* Check if this port is not been used as console */
  consname = Console_Port_Tbl[ Console_Port_Minor ]->sDeviceName;
  if ( !strcmp(MOUSE_DEVICE, consname) ) {
    printk( "SERIAL MOUSE: port selected as console.(%s)\n", *name );
    rtems_fatal_error_occurred( -1 );
  }

  printk("Mouse Device: %s\n", *name );
  return name;
}
