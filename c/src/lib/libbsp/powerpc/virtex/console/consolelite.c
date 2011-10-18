/*
 *  This file contains the console driver for the xilinx uart lite.
 *
 *  Author: Keith Robertson <kjrobert@alumni.uwaterloo.ca>
 *  COPYRIGHT (c) 2005 by Linn Products Ltd, Scotland.
 *
 *  Derived from libbsp/no_cpu/no_bsp/console.c and therefore also:
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <bsp/irq.h>

#include <bsp.h>
#include <libchip/serial.h>
#include <libchip/sersupp.h>

#include RTEMS_XPARAMETERS_H

/* Status Register Masks */
#define PARITY_ERROR       0x80 /* Parity Error */
#define FRAME_ERROR        0x40 /* Frame Error */
#define OVERRUN_ERROR      0x20 /* Overrun Error */
#define STATUS_REG_ERROR_MASK ( PARITY_ERROR | FRAME_ERROR | OVERRUN_ERROR )

#define INTR_ENABLED       0x10 /* Interrupts are enabled */
#define TX_FIFO_FULL       0x08 /* Transmit FIFO is full */
#define TX_FIFO_EMPTY      0x04 /* Transmit FIFO is empty */
#define RX_FIFO_FULL       0x02 /* Receive FIFO is full */
#define RX_FIFO_VALID_DATA 0x01 /* Receive FIFO has valid data */
/* Control Register Masks*/
#define ENABLE_INTR        0x10 /* Enable interrupts */
#define RST_RX_FIFO        0x02 /* Reset and clear RX FIFO */
#define RST_TX_FIFO        0x01 /* Reset and clear TX FIFO */

/* General Defines */
#define TX_FIFO_SIZE       16
#define RX_FIFO_SIZE       16




#define RECV_REG 0
#define TRAN_REG 4
#define STAT_REG 8
#define CTRL_REG 12



RTEMS_INLINE_ROUTINE uint32_t xlite_uart_control(uint32_t base)
{
  uint32_t c = *((volatile uint32_t*)(base+CTRL_REG));
  return c;
}


RTEMS_INLINE_ROUTINE uint32_t xlite_uart_status(uint32_t base)
{
  uint32_t c = *((volatile uint32_t*)(base+STAT_REG));
  return c;
}


RTEMS_INLINE_ROUTINE uint32_t xlite_uart_read(uint32_t base)
{
  uint32_t c = *((volatile uint32_t*)(base+RECV_REG));
  return c;
}


RTEMS_INLINE_ROUTINE void xlite_uart_write(uint32_t base, char ch)
{
  *(volatile uint32_t*)(base+TRAN_REG) = (uint32_t)ch;
  return;
}



int xlite_write_char(uint32_t base, char ch)
{
   uint32_t  retrycount= 0, idler, status;

   while( ((status = xlite_uart_status(base)) & TX_FIFO_FULL) != 0 )
   {
      ++retrycount;

      /* uart tx is busy */
      if( retrycount == 0x4000 )
      {
	 /* retrycount is arbitrary- just make it big enough so the uart is sure to be timed out before it trips */
	 return -1;
      }

      /* spin for a bit so we can sample the register rather than
       * continually reading it */
      for( idler= 0; idler < 0x2000; idler++);
   }

   xlite_uart_write(base, ch);

   return 1;
}











void xlite_init (int minor )
{
   uint32_t base = Console_Port_Tbl[minor]->ulCtrlPort1;

   /* clear status register */
   *((volatile uint32_t*)(base+STAT_REG)) = 0;

   /* clear control register; reset fifos & interrupt enable */
   *((volatile uint32_t*)(base+CTRL_REG)) = RST_RX_FIFO | RST_TX_FIFO;
}


int xlite_open(
  int      major,
  int      minor,
  void    *arg
)
{
   uint32_t base = Console_Port_Tbl[minor]->ulCtrlPort1;

   /* the lite uarts have hardcoded baud & serial parms so no port
    * conditioning is needed.  We're running polled so no interrupt
    * enables either */

   /* clear status register */
   *((volatile uint32_t*)(base+STAT_REG)) = 0;

   /* clear control register; reset fifos & disable interrupts */
   *((volatile uint32_t*)(base+CTRL_REG)) = RST_RX_FIFO | RST_TX_FIFO;

   return RTEMS_SUCCESSFUL;
}


int xlite_close(
  int      major,
  int      minor,
  void    *arg
)
{
   /* no shutdown protocol necessary */
   return RTEMS_SUCCESSFUL;
}



int xlite_read_polled (int minor )
{
   uint32_t base = Console_Port_Tbl[minor]->ulCtrlPort1;

   unsigned int status = xlite_uart_status(base);

   if(status & RX_FIFO_VALID_DATA)
      return (int)xlite_uart_read(base);
   else
      return -1;
}




ssize_t xlite_write_buffer_polled(
  int         minor,
  const char *buf,
  size_t      len
)
{
   uint32_t base = Console_Port_Tbl[minor]->ulCtrlPort1;
   int nwrite = 0;

   /*
    * poll each byte in the string out of the port.
    */
   while (nwrite < len)
   {
      if( xlite_write_char(base, *buf++) < 0 ) break;
      nwrite++;
   }

   /*
    * return the number of bytes written.
    */
   return nwrite;
}


void xlite_write_char_polled(
  int   minor,
  char  c
)
{
   uint32_t base = Console_Port_Tbl[minor]->ulCtrlPort1;
   xlite_write_char(base, c);
   return;
}



int xlite_set_attributes(int minor, const struct termios *t)
{
   return RTEMS_SUCCESSFUL;
}







console_fns xlite_fns_polled =
{
  libchip_serial_default_probe,        /* deviceProbe */
  xlite_open,                          /* deviceFirstOpen */
  xlite_close,                         /* deviceLastClose */
  xlite_read_polled,                   /* deviceRead */
  xlite_write_buffer_polled,           /* deviceWrite */
  xlite_init,                          /* deviceInitialize */
  xlite_write_char_polled,             /* deviceWritePolled */
  xlite_set_attributes,                /* deviceSetAttributes */
  FALSE,                               /* deviceOutputUsesInterrupts */
};






/*
** Set ulCtrlPort1 to the base address of each UART Lite instance.  Set in vhdl model.
*/


console_tbl     Console_Configuration_Ports[] = {
{
  "/dev/ttyS0",                             /* sDeviceName */
   SERIAL_CUSTOM,                           /* deviceType */
   &xlite_fns_polled,                      /* pDeviceFns */
   NULL,                                   /* deviceProbe, assume it is there */
   NULL,                                   /* pDeviceFlow */
   16,                                     /* ulMargin */
   8,                                      /* ulHysteresis */
   (void *) NULL,               /* NULL */ /* pDeviceParams */
   STDIN_BASEADDRESS,                      /* ulCtrlPort1 */
   0,                                      /* ulCtrlPort2 */
   0,                                      /* ulDataPort */
   NULL,                                   /* getRegister */
   NULL,                                   /* setRegister */
   NULL, /* unused */                      /* getData */
   NULL, /* unused */                      /* setData */
   0,                                      /* ulClock */
   0                                       /* ulIntVector -- base for port */
},
{
  "/dev/ttyS1",                             /* sDeviceName */
   SERIAL_CUSTOM,                           /* deviceType */
   &xlite_fns_polled,                       /* pDeviceFns */
   NULL,                                   /* deviceProbe, assume it is there */
   NULL,                                   /* pDeviceFlow */
   16,                                     /* ulMargin */
   8,                                      /* ulHysteresis */
   (void *) NULL,               /* NULL */ /* pDeviceParams */
   0x40610000,                             /* ulCtrlPort1 */
   0,                                      /* ulCtrlPort2 */
   0,                                      /* ulDataPort */
   NULL,                                   /* getRegister */
   NULL,                                   /* setRegister */
   NULL, /* unused */                      /* getData */
   NULL, /* unused */                      /* setData */
   0,                                      /* ulClock */
   0                                       /* ulIntVector -- base for port */
},
{
  "/dev/ttyS2",                             /* sDeviceName */
   SERIAL_CUSTOM,                           /* deviceType */
   &xlite_fns_polled,                       /* pDeviceFns */
   NULL,                                   /* deviceProbe, assume it is there */
   NULL,                                   /* pDeviceFlow */
   16,                                     /* ulMargin */
   8,                                      /* ulHysteresis */
   (void *) NULL,               /* NULL */ /* pDeviceParams */
   0x40620000,                             /* ulCtrlPort1 */
   0,                                      /* ulCtrlPort2 */
   0,                                      /* ulDataPort */
   NULL,                                   /* getRegister */
   NULL,                                   /* setRegister */
   NULL, /* unused */                      /* getData */
   NULL, /* unused */                      /* setData */
   0,                                      /* ulClock */
   0                                       /* ulIntVector -- base for port */
},
{
  "/dev/ttyS3",                             /* sDeviceName */
   SERIAL_CUSTOM,                           /* deviceType */
   &xlite_fns_polled,                       /* pDeviceFns */
   NULL,                                   /* deviceProbe, assume it is there */
   NULL,                                   /* pDeviceFlow */
   16,                                     /* ulMargin */
   8,                                      /* ulHysteresis */
   (void *) NULL,               /* NULL */ /* pDeviceParams */
   0x40630000,                             /* ulCtrlPort1 */
   0,                                      /* ulCtrlPort2 */
   0,                                      /* ulDataPort */
   NULL,                                   /* getRegister */
   NULL,                                   /* setRegister */
   NULL, /* unused */                      /* getData */
   NULL, /* unused */                      /* setData */
   0,                                      /* ulClock */
   0                                       /* ulIntVector -- base for port */
}
};




#define NUM_CONSOLE_PORTS \
  (sizeof(Console_Configuration_Ports)/sizeof(console_tbl))

unsigned long Console_Configuration_Count = NUM_CONSOLE_PORTS;


#include <rtems/bspIo.h>

void outputChar(char ch)
{
  if (ch == '\n') {
    xlite_write_char_polled( 0, '\r' );
  }
   xlite_write_char_polled( 0, ch );
}

int inputChar(void)
{
   return xlite_read_polled(0);
}

BSP_output_char_function_type BSP_output_char = outputChar;
BSP_polling_getchar_function_type BSP_poll_char = inputChar;


