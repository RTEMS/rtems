/*
 * This file contains the TTY driver table. The implementation is
 * based on libchip/serial drivers, but it uses internal SHx SCI so
 * the implementation of the driver is placed in
 * lib/libcpu/sh/sh7045/sci instead of libchip/serial.
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <bsp.h>
#include <libchip/serial.h>
#include <libchip/sersupp.h>
#include <sh/sci_termios.h>
#include <rtems/score/sh_io.h>
#include <rtems/score/ispsh7045.h>
#include <rtems/score/iosh7045.h>
#include <rtems/termiostypes.h>

/*
 * Function set for interrupt enabled termios console
 */
console_fns sh_sci_fns =
{
    libchip_serial_default_probe, /* deviceProbe */
    sh_sci_first_open,            /* deviceFirstOpen */
    NULL,                         /* deviceLastClose */
    NULL,                         /* deviceRead */
    sh_sci_write_support_int,     /* deviceWrite */
    sh_sci_initialize_interrupts, /* deviceInitialize */
    sh_sci_write_polled,          /* deviceWritePolled */
    sh_sci_set_attributes,        /* deviceSetAttributes */
    TERMIOS_IRQ_DRIVEN           /* deviceOutputUsesInterrupts */
};

/*
 * Function set for polled termios console
 */
console_fns sh_sci_fns_polled =
{
    libchip_serial_default_probe, /* deviceProbe */
    sh_sci_first_open,            /* deviceFirstOpen */
    sh_sci_last_close,            /* deviceLastClose */
    sh_sci_inbyte_nonblocking_polled, /* deviceRead */
    sh_sci_write_support_polled,  /* deviceWrite */
    sh_sci_init,                  /* deviceInitialize */
    sh_sci_write_polled,          /* deviceWritePolled */
    sh_sci_set_attributes,        /* deviceSetAttributes */
    TERMIOS_POLLED                /* deviceOutputUsesInterrupts */
};

#if 1 /* (CONSOLE_USE_INTERRUPTS) */
#define SCI_FUNCTIONS &sh_sci_fns
#else
#define SCI_FUNCTIONS &sh_sci_fns_polled
#endif

static const struct termios term1 = {
    0,
    0,
    B9600 | CS8,
    0,
    0,
    { 0 }
};

static const struct termios term2 = {
    0,
    0,
    B115200 | CS8,
    0,
    0,
    { 0 }
};

console_tbl	Console_Configuration_Ports[] = {
    {
        "/dev/sci0",            /* sDeviceName */
        SERIAL_CUSTOM,          /* deviceType */
        SCI_FUNCTIONS,          /* pDeviceFns */
        NULL,                   /* deviceProbe */
        NULL,                   /* pDeviceFlow */
        16,                     /* ulMargin */
        8,                      /* ulHysteresis */
        (void *)&term1,         /* baud rate */    /* pDeviceParams */
        SCI_SMR0,               /* ulCtrlPort1 */
        3,                      /* ulCtrlPort2 as IRQ priority level*/
        TXI0_ISP_V,             /* ulDataPort as TX end vector number*/
        NULL,                   /* unused */       /* getRegister */
        NULL,                   /* unused */	   /* setRegister */
        NULL,                   /* unused */       /* getData */
        NULL,                   /* unused */       /* setData */
        0,                      /* ulClock */
        RXI0_ISP_V,             /* ulIntVector as RX end vector number*/
    },
    {
        "/dev/sci1",            /* sDeviceName */
        SERIAL_CUSTOM,          /* deviceType */
        SCI_FUNCTIONS,          /* pDeviceFns */
        NULL,                   /* deviceProbe */
        NULL,                   /* pDeviceFlow */
        16,                     /* ulMargin */
        8,                      /* ulHysteresis */
        (void *)&term2,         /* baud rate */    /* pDeviceParams */
        SCI_SMR1,               /* ulCtrlPort1 */
        3,                      /* ulCtrlPort2 as IRQ priority level*/
        TXI1_ISP_V,             /* ulDataPort as TX end vector number*/
        NULL,                   /* unused */       /* getRegister */
        NULL,                   /* unused */       /* setRegister */
        NULL,                   /* unused */       /* getData */
        NULL,                   /* unused */       /* setData */
        0,                      /* ulClock */
        RXI1_ISP_V,             /* ulIntVector as RX end vector number*/
    }
};

/*
 *  Declare some information used by the console driver
 */

#define NUM_CONSOLE_PORTS (sizeof(Console_Configuration_Ports)/sizeof(console_tbl))

unsigned long  Console_Configuration_Count =  NUM_CONSOLE_PORTS;
