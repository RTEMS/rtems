/*
 *  This file contains the libchip configuration information
 *  to instantiate the libchip driver for the on-CPU DUART
 *  and any other serial ports in the system.
 *
 *  COPYRIGHT (c) 1989-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <bsp.h> 
 
#include <libchip/serial.h>
#include <libchip/mg5uart.h>

/* #define CONSOLE_USE_INTERRUPTS */

#ifdef CONSOLE_USE_INTERRUPTS
#define MG5UART_FUNCTIONS &mg5uart_fns
#else
#define MG5UART_FUNCTIONS &mg5uart_fns_polled
#endif


console_tbl     Console_Port_Tbl[] = {
{
  "/dev/com0",                             /* sDeviceName */
   SERIAL_MG5UART,                         /* deviceType */
   MG5UART_FUNCTIONS,                      /* pDeviceFns */
   NULL,                                   /* deviceProbe, assume it is there */
   NULL,                                   /* pDeviceFlow */
   16,                                     /* ulMargin */
   8,                                      /* ulHysteresis */
   (void *) NULL,               /* NULL */ /* pDeviceParams */
   MONGOOSEV_PERIPHERAL_COMMAND_REGISTER,  /* ulCtrlPort1 */
   MONGOOSEV_UART0_BASE,                   /* ulCtrlPort2 */
   MG5UART_UART0,                          /* ulDataPort */
   NULL,                                   /* getRegister */
   NULL,                                   /* setRegister */
   NULL, /* unused */                      /* getData */
   NULL, /* unused */                      /* setData */
   CLOCK_RATE,                             /* ulClock */
   MONGOOSEV_IRQ_UART0_RX_FRAME_ERROR      /* ulIntVector -- base for port */
},
{
  "/dev/com1",                             /* sDeviceName */
   SERIAL_MG5UART,                         /* deviceType */
   MG5UART_FUNCTIONS,                      /* pDeviceFns */
   NULL,                                   /* deviceProbe, assume it is there */
   NULL,                                   /* pDeviceFlow */
   16,                                     /* ulMargin */
   8,                                      /* ulHysteresis */
   (void *) NULL,               /* NULL */ /* pDeviceParams */
   MONGOOSEV_PERIPHERAL_COMMAND_REGISTER,  /* ulCtrlPort1 */
   MONGOOSEV_UART1_BASE,                   /* ulCtrlPort2 */
   MG5UART_UART1,                          /* ulDataPort */
   NULL,                                   /* getRegister */
   NULL,                                   /* setRegister */
   NULL, /* unused */                      /* getData */
   NULL, /* unused */                      /* setData */
   CLOCK_RATE,                             /* ulClock */
   MONGOOSEV_IRQ_UART1_RX_FRAME_ERROR      /* ulIntVector -- base for port */
},
};

/*
 *  Declare some information used by the console driver
 */

#define NUM_CONSOLE_PORTS (sizeof(Console_Port_Tbl)/sizeof(console_tbl))

unsigned long  Console_Port_Count = NUM_CONSOLE_PORTS;

console_data  Console_Port_Data[NUM_CONSOLE_PORTS];

rtems_device_minor_number  Console_Port_Minor;


/*
 *  printk() support that simply routes printk to stderr
 */

#include <rtems/bspIo.h>

void GENMG5_output_char(char c) { write( 2, &c, 1 ); }
  
BSP_output_char_function_type           BSP_output_char = GENMG5_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;
