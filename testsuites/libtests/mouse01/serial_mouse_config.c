/*
 *
 * MODULE DESCRIPTION:
 * This module implements the RTEMS drivers for the PC serial ports
 * as /dev/ttyS1 for COM1 and /dev/ttyS2 as COM2. If one of the ports
 * is used as the console, this driver would fail to initialize.
 *
 * This code was based on the console driver. It is based on the
 * current termios framework. This is just a shell around the
 * termios support.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tmacros.h"
#include <rtems/serial_mouse.h>
#include "termios_testdriver_intr.h"

extern const char *Mouse_Type_Short;

bool bsp_get_serial_mouse_device(
  const char **name,
  const char **type
)
{
  *name = TERMIOS_TEST_DRIVER_DEVICE_NAME;
  *type = Mouse_Type_Short;

  return true;
}
