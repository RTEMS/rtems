/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems/serial_mouse.h>

bool bsp_get_serial_mouse_device(
  const char **name,
  const char **type
)
{
  *name = SERIAL_MOUSE_DEVICE_PS2;
  *type = "ps2";

  return true;
}
