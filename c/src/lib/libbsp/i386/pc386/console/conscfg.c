/*
 *  This file contains the libchip configuration information
 *  to instantiate the libchip driver for the VGA console
 *  and serial ports on a PC.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <unistd.h> /* write */

#include <bsp.h>
#include <libchip/serial.h>
#include <libchip/ns16550.h>
#include "vgacons.h"
#include <bsp/irq.h>
#include <rtems/pci.h>

#define VGA_CONSOLE_FUNCTIONS  &vgacons_fns
#if 0
#define COM_CONSOLE_FUNCTIONS  &ns16550_fns_polled
#else
#define COM_CONSOLE_FUNCTIONS  &ns16550_fns
#endif

/*
 * Base IO for UART
 */
#define COM1_BASE_IO  0x3F8
#define COM2_BASE_IO  0x3E8
#define COM3_BASE_IO  0x2F8
#define COM4_BASE_IO  0x2E8

#define CLOCK_RATE     (115200 * 16)

uint8_t com_get_register(uint32_t addr,uint8_t i)
{
  register uint8_t val;
  
  inport_byte( (addr + i),val );
  return val;
}

void com_set_register(uint32_t addr,uint8_t i, uint8_t val)
{
  outport_byte( (addr+i),val );
}

console_tbl     Console_Configuration_Ports[] = {
  {
    "/dev/vgacons",                        /* sDeviceName */
    VGA_CONSOLE,                           /* deviceType */
    VGA_CONSOLE_FUNCTIONS,                 /* pDeviceFns */
    vgacons_probe,                         /* deviceProbe */
    NULL,                                  /* pDeviceFlow */
    16,                                    /* ulMargin */
    8,                                     /* ulHysteresis */
    (void *) NULL,              /* NULL */ /* pDeviceParams */
    0x00000000,                            /* ulCtrlPort1 */
    0x00000000,                            /* ulCtrlPort2 */
    0x00000000,                            /* ulDataPort */
    NULL,                                  /* getRegister */
    NULL,                                  /* setRegister */
    NULL,/* unused */                      /* getData */
    NULL,/* unused */                      /* setData */
    0X0,                                   /* ulClock */
    0x0                                     /* ulIntVector -- base for port */
  },
  {
    "/dev/com1",                           /* sDeviceName */
    SERIAL_NS16550,                        /* deviceType */
    COM_CONSOLE_FUNCTIONS,                 /* pDeviceFns */
    NULL,                                  /* deviceProbe, assume it is there */
    NULL,                                  /* pDeviceFlow */
    16,                                    /* ulMargin */
    8,                                     /* ulHysteresis */
    (void *) 9600,         /* Baud Rate */ /* pDeviceParams */
    COM1_BASE_IO,                          /* ulCtrlPort1 */
    0x00000000,                            /* ulCtrlPort2 */
    COM1_BASE_IO,                          /* ulDataPort */
    com_get_register,                      /* getRegister */
    com_set_register,                      /* setRegister */
    NULL,/* unused */                      /* getData */
    NULL,/* unused */                      /* setData */
    CLOCK_RATE,                            /* ulClock */
    BSP_UART_COM1_IRQ                      /* ulIntVector -- base for port */
  },
  {
    "/dev/com2",                           /* sDeviceName */
    SERIAL_NS16550,                        /* deviceType */
    COM_CONSOLE_FUNCTIONS,                 /* pDeviceFns */
    NULL,                                  /* deviceProbe, assume it is there */
    NULL,                                  /* pDeviceFlow */
    16,                                    /* ulMargin */
    8,                                     /* ulHysteresis */
    (void *) 9600,         /* Baud Rate */ /* pDeviceParams */
    COM2_BASE_IO,                          /* ulCtrlPort1 */
    0x00000000,                            /* ulCtrlPort2 */
    COM2_BASE_IO,                          /* ulDataPort */
    com_get_register,                      /* getRegister */
    com_set_register,                      /* setRegister */
    NULL,/* unused */                      /* getData */
    NULL,/* unused */                      /* setData */
    CLOCK_RATE,                            /* ulClock */
    BSP_UART_COM2_IRQ                      /* ulIntVector -- base for port */
  },

  {
    "/dev/com3",                           /* sDeviceName */
    SERIAL_NS16550,                        /* deviceType */
    COM_CONSOLE_FUNCTIONS,                 /* pDeviceFns */
    NULL,                                  /* deviceProbe, assume it is there */
    NULL,                                  /* pDeviceFlow */
    16,                                    /* ulMargin */
    8,                                     /* ulHysteresis */
    (void *) 9600,         /* Baud Rate */ /* pDeviceParams */
    COM3_BASE_IO,                          /* ulCtrlPort1 */
    0x00000000,                            /* ulCtrlPort2 */
    COM3_BASE_IO,                          /* ulDataPort */
    com_get_register,                      /* getRegister */
    com_set_register,                      /* setRegister */
    NULL,/* unused */                      /* getData */
    NULL,/* unused */                      /* setData */
    CLOCK_RATE,                            /* ulClock */
    BSP_UART_COM3_IRQ                      /* ulIntVector -- base for port */
  },

  {
    "/dev/com4",                           /* sDeviceName */
    SERIAL_NS16550,                        /* deviceType */
    COM_CONSOLE_FUNCTIONS,                 /* pDeviceFns */
    NULL,                                  /* deviceProbe, assume it is there */
    NULL,                                  /* pDeviceFlow */
    16,                                    /* ulMargin */
    8,                                     /* ulHysteresis */
    (void *) 9600,         /* Baud Rate */ /* pDeviceParams */
    COM4_BASE_IO,                          /* ulCtrlPort1 */
    0x00000000,                            /* ulCtrlPort2 */
    COM4_BASE_IO,                          /* ulDataPort */
    com_get_register,                      /* getRegister */
    com_set_register,                      /* setRegister */
    NULL,/* unused */                      /* getData */
    NULL,/* unused */                      /* setData */
    CLOCK_RATE,                            /* ulClock */
    BSP_UART_COM4_IRQ                      /* ulIntVector -- base for port */
  },

};

/*
 *  Define a variable that contains the number of statically configured
 *  console devices.
 */
unsigned long  Console_Configuration_Count = \
    (sizeof(Console_Configuration_Ports)/sizeof(console_tbl));
