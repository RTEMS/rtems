/*
 *  This file contains the TTY driver table.
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <bspopts.h>

#include <libchip/serial.h>
#include <libchip/mc68681.h>

/*
 *  Fix these for the simulator
 */

#define MC68681_PORT_CONFIG  MC68681_XBRG_IGNORED
#define MC68681_PROBE        libchip_serial_default_probe
#define MC68681_VECTOR       0
#define MC68681_BASE_ADDRESS 0x71001
#define MC68681_DATA_ADDRESS 0x71007

/*
 *  Based on BSP configuration information decide whether to do polling IO
 *  or interrupt driven IO.
 */

#if (CONSOLE_USE_INTERRUPTS)
#define MC68681_FUNCTIONS &mc68681_fns
#else
#define MC68681_FUNCTIONS &mc68681_fns_polled
#endif

console_tbl  Console_Configuration_Ports[] = {
  {
    "/dev/com0",                        /* sDeviceName */
    SERIAL_MC68681,                     /* deviceType */
    MC68681_FUNCTIONS,                  /* pDeviceFns */
    NULL,                               /* deviceProbe */
    NULL,                               /* pDeviceFlow */
    16,                                 /* ulMargin */
    8,                                  /* ulHysteresis */
    (void *)9600,                       /* pDeviceParams */
    MC68681_BASE_ADDRESS,               /* ulCtrlPort1 */
    MC68681_BASE_ADDRESS,               /* ulCtrlPort2 */
    MC68681_DATA_ADDRESS,               /* ulDataPort */
    mc68681_get_register_2,             /* getRegister */
    mc68681_set_register_2,             /* setRegister */
    NULL, /* unused */                  /* getData */
    NULL, /* unused */                  /* setData */
    (uint32_t)mc68681_baud_rate_table,  /* ulClock */
    MC68681_VECTOR                      /* ulIntVector */
  }
};

/*
 *  Declare some information used by the console driver
 */

#define NUM_CONSOLE_PORTS (sizeof(Console_Configuration_Ports)/sizeof(console_tbl))

unsigned long  Console_Configuration_Count = NUM_CONSOLE_PORTS;
