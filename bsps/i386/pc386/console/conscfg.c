/**
 * @file
 *
 * This file contains the libchip configuration information
 * to instantiate the libchip driver for the VGA console
 * and serial ports on a PC.
 */

/*
 *  COPYRIGHT (c) 1989-2014, 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <bsp/bspimpl.h>
#include <libchip/serial.h>
#include <libchip/ns16550.h>
#if BSP_ENABLE_VGA
#include <rtems/vgacons.h>
#endif
#include <bsp/irq.h>
#include "../../shared/dev/serial/legacy-console.h"

#if BSP_ENABLE_VGA
#define VGA_CONSOLE_FUNCTIONS  &vgacons_fns
#endif

#if BSP_ENABLE_COM1_COM4
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

  #define CLOCK_RATE    (115200 * 16)

  static uint8_t com_get_register(uint32_t addr, uint8_t i)
  {
    register uint8_t val;

    inport_byte( (addr + i), val );
    return val;
  }

  static void com_set_register(uint32_t addr, uint8_t i, uint8_t val)
  {
    outport_byte( (addr + i), val );
  }
#endif

/*
 * Default to the PC VGA console if present and configured.
 */
console_tbl Console_Configuration_Ports[] = {
#if BSP_ENABLE_VGA
  /*
   * If present the VGA console must always be minor 0.
   * See console_control.
   */
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
    0x0,                                   /* ulClock */
    0x0                                    /* ulIntVector -- base for port */
  },
#endif
};

unsigned long Console_Configuration_Count =
    (sizeof(Console_Configuration_Ports)/sizeof(console_tbl));

static console_tbl Legacy_Ports[] = {
#if BSP_ENABLE_COM1_COM4
  {
    "/dev/com1",                           /* sDeviceName */
    SERIAL_NS16550,                        /* deviceType */
    COM_CONSOLE_FUNCTIONS,                 /* pDeviceFns */
    NULL,                                  /* deviceProbe */
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
    NULL,                                  /* deviceProbe */
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
    NULL,                                  /* deviceProbe */
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
    NULL,                                  /* deviceProbe */
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
#endif
};

#define Legacy_Port_Count \
    (sizeof(Legacy_Ports)/sizeof(console_tbl))

void legacy_uart_probe(void)
{
#if BSP_ENABLE_COM1_COM4
  const char *opt;
  /*
   * Check the command line to see if com1-com4 are disabled.
   */
  opt = bsp_cmdline_arg("--disable-com1-com4");
  if ( opt ) {
    printk( "COM1-COM4: disabled\n" );
  } else {
    if (Legacy_Port_Count) {
      printk("Legacy UART Ports: COM1-COM4\n");
      console_register_devices( Legacy_Ports, Legacy_Port_Count );
    }
  }
#endif
}
