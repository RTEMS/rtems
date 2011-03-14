/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <bsp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* select which serial port the mouse is connected to */
/* XXX - Hook these somewhere */
#ifdef   SERIAL_MOUSE_COM1
  #define SERIAL_MOUSE_COM  1
#elif defined(SERIAL_MOUSE_COM2)
  #define SERIAL_MOUSE_COM  2
#else
  /* Select Default to be COM1  */
  #define SERIAL_MOUSE_COM  1
#endif

extern int BSPConsolePort;

bool bsp_get_serial_mouse_device(
  const char **name,
  const char **type
)
{
  #ifdef SERIAL_MOUSE_COM2
    *name = "/dev/ttyS2";
  #else
    *name = "/dev/ttyS1";
  #endif
  
  *type = "ms";

  /* Check if this port is not been used as console */
  /* XXX configure the serial port, take boot args additionally */
  if ( BSPConsolePort == SERIAL_MOUSE_COM ) {
    printk( "SERIAL MOUSE: port selected as console.(%s)\n", *name );
    rtems_fatal_error_occurred( -1 );
  }

  printk("Mouse Device: %s\n", *name );
  return name;
}
