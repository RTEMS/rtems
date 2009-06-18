/*
 *  Console driver for CSB337
 *
 *  This driver uses the shared console driver in 
 *  ...../libbsp/shared/console.c
 *
 *  Copyright (c) 2003 by Cogent Computer Systems
 *  Written by Jay Monkman <jtm@lopingdog.com> 
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  FrameBuffer Console Device Support added By Joel Sherrill, 2009.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <termios.h>
#include <rtems/bspIo.h>

#include <at91rm9200.h>
#include <at91rm9200_dbgu.h>
#include <libchip/serial.h>
#include <libchip/sersupp.h>
#include <bspopts.h>

/* rtems console uses the following minor number */
rtems_device_minor_number  Console_Port_Minor = 0;
extern console_fns dbgu_fns;
#if ENABLE_LCD
  extern console_fns fbcons_fns;
  #define LCD_DEVICE     1
#else
  #define LCD_DEVICE     0
#endif

#if ENABLE_UMON_CONSOLE
  extern console_fns umoncons_fns;
  #define UMON_CONSOLE_DEVICE 1
#else
  #define UMON_CONSOLE_DEVICE 0
#endif

#define NUM_DEVS   (1 + LCD_DEVICE + UMON_CONSOLE_DEVICE)

/* These are used by code in console.c */
unsigned long Console_Port_Count = NUM_DEVS;
console_data  Console_Port_Data[NUM_DEVS];

/* 
 * There's one item in array for each UART.
 *
 * Some of these fields are marked "NOT USED". They are not used
 * by console.c, but may be used by drivers in libchip
 *
 * when we add other types of UARTS we will need to move this
 * structure to a generic uart.c file with only this in it
 */
console_tbl Console_Port_Tbl[] = {
  {
    "/dev/console",    /* sDeviceName */
    SERIAL_CUSTOM,     /* deviceType */
    &dbgu_fns,         /* pDeviceFns */
    NULL,              /* deviceProbe */
    NULL,              /* pDeviceFlow */
    0,                 /* ulMargin - NOT USED */
    0,                 /* ulHysteresis - NOT USED */
    NULL,              /* pDeviceParams */
    DBGU_BASE,         /* ulCtrlPort1  - Pointer to DBGU regs */
    0,                 /* ulCtrlPort2  - NOT USED */
    0,                 /* ulDataPort  - NOT USED */
    NULL,              /* getRegister - NOT USED */
    NULL,              /* setRegister - NOT USED */
    NULL,              /* getData - NOT USED */
    NULL,              /* setData - NOT USED */
    0,                 /* ulClock - NOT USED */
    0                  /* ulIntVector - NOT USED */
  },
#if ENABLE_LCD
  {
    "/dev/fbcons",     /* sDeviceName */
    SERIAL_CUSTOM,     /* deviceType */
    &fbcons_fns,       /* pDeviceFns */
    NULL,              /* deviceProbe */
    NULL,              /* pDeviceFlow */
    0,                 /* ulMargin - NOT USED */
    0,                 /* ulHysteresis - NOT USED */
    NULL,              /* pDeviceParams */
    0,                 /* ulCtrlPort1  - Pointer to DBGU regs */
    0,                 /* ulCtrlPort2  - NOT USED */
    0,                 /* ulDataPort  - NOT USED */
    NULL,              /* getRegister - NOT USED */
    NULL,              /* setRegister - NOT USED */
    NULL,              /* getData - NOT USED */
    NULL,              /* setData - NOT USED */
    0,                 /* ulClock - NOT USED */
    0                  /* ulIntVector - NOT USED */
  },
#endif
#if ENABLE_UMON_CONSOLE
  {
    "/dev/umon",       /* sDeviceName */
    SERIAL_CUSTOM,     /* deviceType */
    &umoncons_fns,     /* pDeviceFns */
    NULL,              /* deviceProbe */
    NULL,              /* pDeviceFlow */
    0,                 /* ulMargin - NOT USED */
    0,                 /* ulHysteresis - NOT USED */
    NULL,              /* pDeviceParams */
    0,                 /* ulCtrlPort1  - Pointer to UMON regs */
    0,                 /* ulCtrlPort2  - NOT USED */
    0,                 /* ulDataPort  - NOT USED */
    NULL,              /* getRegister - NOT USED */
    NULL,              /* setRegister - NOT USED */
    NULL,              /* getData - NOT USED */
    NULL,              /* setData - NOT USED */
    0,                 /* ulClock - NOT USED */
    0                  /* ulIntVector - NOT USED */
  }
#endif
};


console_tbl *BSP_get_uart_from_minor(int minor)
{
    return &Console_Port_Tbl[minor];
}
