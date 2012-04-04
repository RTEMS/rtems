/**
 *  @file
 *
 *  This file contains the libchip configuration information
 *  to instantiate the libchip driver for the serial ports.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
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
#include <rtems/pci.h>
#include <bsp/irq.h>

#if 1
#define COM_CONSOLE_FUNCTIONS  &ns16550_fns_polled
#else
#define COM_CONSOLE_FUNCTIONS  &ns16550_fns
#endif

/*
 * Base IO for UART
 */
#define COM1_BASE_IO  0x3F8
#define COM2_BASE_IO  0x3E8

// #define CLOCK_RATE     368640
#define CLOCK_RATE     (115200 * 16)

#define COM_IO_BASE_ADDRESS   (0xa0000000UL | 0x18000000UL)

uint8_t com_get_register(uint32_t addr, uint8_t i);
void com_set_register(uint32_t addr, uint8_t i, uint8_t val);
uint8_t tty2_get_register(uint32_t addr, uint8_t i);
void tty2_set_register(uint32_t addr, uint8_t i, uint8_t val);


uint8_t com_get_register(uint32_t addr, uint8_t i)
{
  uint8_t val;
  volatile uint8_t *ptr;
  ptr = (volatile uint8_t *) COM_IO_BASE_ADDRESS;
  ptr += addr;
  ptr += i;
  val = *ptr;

  return val;
}

void com_set_register(uint32_t addr, uint8_t i, uint8_t val)
{
  volatile uint8_t *ptr;

  ptr = (volatile uint8_t *) COM_IO_BASE_ADDRESS;
  ptr += addr;
  ptr += i;
  *ptr = val;
}

uint8_t tty2_get_register(uint32_t addr, uint8_t i)
{
  uint8_t val;
  volatile uint8_t *ptr;

  ptr = (volatile uint8_t *) COM_IO_BASE_ADDRESS;
  ptr += addr;
  ptr += (i * 8);
  val = *ptr;

  return val;
}

void tty2_set_register(uint32_t addr, uint8_t i, uint8_t val)
{
  volatile uint8_t *ptr;

  ptr = (volatile uint8_t *) COM_IO_BASE_ADDRESS;
  ptr += addr;
  ptr += (i * 8);
  *ptr = val;
}

console_tbl     Console_Configuration_Ports[] = {
  {
    "/dev/tty0",                          /* sDeviceName */
    SERIAL_NS16550,                       /* deviceType */
    COM_CONSOLE_FUNCTIONS,                /* pDeviceFns */
    NULL,                                 /* deviceProbe, assume it is there */
    NULL,                                 /* pDeviceFlow */
    16,                                   /* ulMargin */
    8,                                    /* ulHysteresis */
    (void *) 9600,        /* Baud Rate */ /* pDeviceParams */
    COM1_BASE_IO,                         /* ulCtrlPort1 */
    0x00000000,                           /* ulCtrlPort2 */
    COM1_BASE_IO,                         /* ulDataPort */
    com_get_register,                     /* getRegister */
    com_set_register,                     /* setRegister */
    NULL,/* unused */                     /* getData */
    NULL,/* unused */                     /* setData */
    CLOCK_RATE,                           /* ulClock */
    MALTA_IRQ_TTY0                        /* ulIntVector -- base for port */
  },
  {
    "/dev/tty1",                          /* sDeviceName */
    SERIAL_NS16550,                       /* deviceType */
    COM_CONSOLE_FUNCTIONS,                /* pDeviceFns */
    NULL,                                 /* deviceProbe, assume it is there */
    NULL,                                 /* pDeviceFlow */
    16,                                   /* ulMargin */
    8,                                    /* ulHysteresis */
    (void *) 9600,        /* Baud Rate */ /* pDeviceParams */
    COM2_BASE_IO,                         /* ulCtrlPort1 */
    0x00000000,                           /* ulCtrlPort2 */
    COM2_BASE_IO,                         /* ulDataPort */
    com_get_register,                     /* getRegister */
    com_set_register,                     /* setRegister */
    NULL,/* unused */                     /* getData */
    NULL,/* unused */                     /* setData */
    CLOCK_RATE,                           /* ulClock */
    MALTA_IRQ_TTY1                        /* ulIntVector -- base for port */
  },
  {
    "/dev/tty2",                          /* sDeviceName */
    SERIAL_NS16550,                       /* deviceType */
    COM_CONSOLE_FUNCTIONS,                /* pDeviceFns */
    NULL,                                 /* deviceProbe, assume it is there */
    NULL,                                 /* pDeviceFlow */
    16,                                   /* ulMargin */
    8,                                    /* ulHysteresis */
    (void *) 9600,        /* Baud Rate */ /* pDeviceParams */
    0,                    /* IGNORED */   /* ulCtrlPort1 */
    0,                    /* IGNORED */   /* ulCtrlPort2 */
    0,                    /* IGNORED */   /* ulDataPort */
    tty2_get_register,                    /* getRegister */
    tty2_set_register,                    /* setRegister */
    NULL,/* unused */                     /* getData */
    NULL,/* unused */                     /* setData */
    CLOCK_RATE,                           /* ulClock */
    MALTA_CPU_INT2                        /* ulIntVector -- base for port */
  },
};

/*
 *  Define a variable that contains the number of statically configured
 *  console devices.
 */
unsigned long  Console_Configuration_Count = \
    (sizeof(Console_Configuration_Ports)/sizeof(console_tbl));
