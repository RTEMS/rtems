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
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/serial_mouse.h>

#include <libchip/serial.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/arm-pl011.h>
#include <bsp/arm-pl050.h>

console_tbl Console_Configuration_Ports[] = {
  {
    .sDeviceName = "/dev/ttyS0",
    .deviceType = SERIAL_CUSTOM,
    .pDeviceFns = &arm_pl011_fns,
    .deviceProbe = NULL,
    .pDeviceFlow = NULL,
    .ulMargin = 10,
    .ulHysteresis = 0,
    .pDeviceParams = (void *) 115200,
    .ulCtrlPort1 = 0x10009000,
    .ulCtrlPort2 = 0,
    .ulDataPort = 0,
    .getRegister = NULL,
    .setRegister = NULL,
    .getData = NULL,
    .setData = NULL,
    .ulClock = 0,
    .ulIntVector = RVPBXA9_IRQ_UART_0
  }, {
    .sDeviceName = SERIAL_MOUSE_DEVICE_PS2,
    .deviceType = SERIAL_CUSTOM,
    .pDeviceFns = &arm_pl050_fns,
    .deviceProbe = NULL,
    .pDeviceFlow = NULL,
    .ulMargin = 10,
    .ulHysteresis = 0,
    .pDeviceParams = (void *) 115200,
    .ulCtrlPort1 = 0x10007000,
    .ulCtrlPort2 = 0,
    .ulDataPort = 0,
    .getRegister = NULL,
    .setRegister = NULL,
    .getData = NULL,
    .setData = NULL,
    .ulClock = 0,
    .ulIntVector = RVPBXA9_IRQ_KMI1
  }
};

unsigned long Console_Configuration_Count =
  RTEMS_ARRAY_SIZE(Console_Configuration_Ports);
