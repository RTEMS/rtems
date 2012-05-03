/*
 *  Console driver for GUMSTIX by Yang Xi <hiyangxi@gmail.com>
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
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <termios.h>
#include <rtems/bspIo.h>

#include <pxa255.h>
#include <libchip/serial.h>
#include <libchip/sersupp.h>

/* How many serial ports? */
#define NUM_DEVS       1

/* These are used by code in console.c */
unsigned long Console_Configuration_Count = NUM_DEVS;

extern console_fns ffuart_fns;

/*
 * There's one item in array for each UART.
 *
 * Some of these fields are marked "NOT USED". They are not used
 * by console.c, but may be used by drivers in libchip
 *
 * when we add other types of UARTS we will need to move this
 * structure to a generic uart.c file with only this in it
 */
console_tbl Console_Configuration_Ports[] = {
    {
        "/dev/com0",       /* sDeviceName */
        SERIAL_CUSTOM,     /* deviceType */
        &ffuart_fns,       /* pDeviceFns */
        NULL,              /* deviceProbe */
        NULL,              /* pDeviceFlow */
        0,                 /* ulMargin - NOT USED */
        0,                 /* ulHysteresis - NOT USED */
        NULL,              /* pDeviceParams */
        FFUART_BASE,       /* ulCtrlPort1  - Pointer to DBGU regs */
        0,                 /* ulCtrlPort2  - NOT USED */
        0,                 /* ulDataPort  - NOT USED */
        NULL,              /* getRegister - NOT USED */
        NULL,              /* setRegister - NOT USED */
        NULL,              /* getData - NOT USED */
        NULL,              /* setData - NOT USED */
        0,                 /* ulClock - NOT USED */
        0                  /* ulIntVector - NOT USED */
    }};


console_tbl *BSP_get_uart_from_minor(int minor)
{
    return Console_Port_Tbl[minor];
}
