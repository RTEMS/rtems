/*
 *  This file contains the PowerPC 405GP console IO package.
 *
 *  Author:	Thomas Doerfler <td@imd.m.isar.de>
 *              IMD Ingenieurbuero fuer Microcomputertechnik
 *
 *  COPYRIGHT (c) 1998 by IMD
 *
 *  Changes from IMD are covered by the original distributions terms.
 *  changes include interrupt support and termios support
 *  for backward compatibility, the original polled driver has been
 *  renamed to console.c.polled
 *
 *  This file has been initially created (polled version) by
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Modifications for spooling (interrupt driven) console driver
 *            by Thomas Doerfler <td@imd.m.isar.de>
 *  for these modifications:
 *  COPYRIGHT (c) 1997 by IMD, Puchheim, Germany.
 *
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies. IMD makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libbsp/no_cpu/no_bsp/console/console.c:
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Modifications for PPC405GP by Dennis Ehlin
 */

#define NO_BSP_INIT

#include <rtems.h>
#include <rtems/libio.h>
#include "../irq/ictrl.h"
#include <stdlib.h>                                     /* for atexit() */



struct async {
/*---------------------------------------------------------------------------+
| Data Register.
+---------------------------------------------------------------------------*/
  unsigned char RBR;	/* 0x00 */
  #define THR   RBR
/*---------------------------------------------------------------------------+
| Interrupt registers
+---------------------------------------------------------------------------*/
  unsigned char IER;    /* 0x01 */
  #define IER_RCV		 0x01
  #define IER_XMT		 0x02
  #define IER_LS		 0x04
  #define IER_MS		 0x08

  unsigned char ISR;    /* 0x02 */
  #define ISR_MS		 0x00
  #define ISR_nIP		 0x01
  #define ISR_Tx		 0x02
  #define ISR_Rx		 0x04
  #define ISR_LS		 0x06
  #define ISR_RxTO		 0x0C
  #define ISR_64BFIFO		 0x20
  #define ISR_FIFOworks		 0x40
  #define ISR_FIFOen		 0x80

/*---------------------------------------------------------------------------+
| FIFO Control registers
+---------------------------------------------------------------------------*/
  #define FCR   ISR
  #define FCR_FE		0x01    /* FIFO enable */
  #define FCR_CRF		0x02    /* Clear receive FIFO */
  #define FCR_CTF		0x04    /* Clear transmit FIFO */
  #define FCR_DMA		0x08    /* DMA mode select */
  #define FCR_F64		0x20    /* Enable 64 byte fifo (16750+) */
  #define FCR_RT14		0xC0    /* Set Rx trigger at 14 */
  #define FCR_RT8		0x80    /* Set Rx trigger at 8 */
  #define FCR_RT4		0x40    /* Set Rx trigger at 4 */
  #define FCR_RT1		0x00    /* Set Rx trigger at 1 */

/*---------------------------------------------------------------------------+
| Baud rate divisor registers
+---------------------------------------------------------------------------*/
  #define DLL   RBR
  #define DLM   IER

/*---------------------------------------------------------------------------+
| Alternate function registers
+---------------------------------------------------------------------------*/
  #define AFR   ISR

/*---------------------------------------------------------------------------+
| Line control Register.
+---------------------------------------------------------------------------*/
  unsigned char LCR;	/* 0x03 */
  #define LCR_WL5		 0x00   /* Word length 5 */
  #define LCR_WL6		 0x01   /* Word length 6 */
  #define LCR_WL7		 0x02   /* Word length 7 */
  #define LCR_WL8		 0x03   /* Word length 8 */

  #define LCR_SB1		 0x00	/* 1 stop bits */
  #define LCR_SB1_5		 0x04	/* 1.5 stop bits , only valid with 5 bit words*/
  #define LCR_SB1_5		 0x04	/* 2 stop bits */

  #define LCR_PN		 0x00	/* Parity NONE */
  #define LCR_PE		 0x0C	/* Parity EVEN */
  #define LCR_PO		 0x08	/* Parity ODD */
  #define LCR_PM		 0x28	/* Forced "mark" parity */
  #define LCR_PS		 0x38	/* Forced "space" parity */

  #define LCR_DL		 0x80	/* Enable baudrate latch */

/*---------------------------------------------------------------------------+
| Modem control Register.
+---------------------------------------------------------------------------*/
  unsigned char MCR;	/* 0x04 */
  #define MCR_DTR		 0x01
  #define MCR_RTS		 0x02
  #define MCR_INT		 0x08	/* Enable interrupts */
  #define MCR_LOOP		 0x10	/* Loopback mode */

/*---------------------------------------------------------------------------+
| Line status Register.
+---------------------------------------------------------------------------*/
  unsigned char LSR;	/* 0x05 */
  #define LSR_RSR		 0x01
  #define LSR_OE		 0x02
  #define LSR_PE		 0x04
  #define LSR_FE		 0x08
  #define LSR_BI		 0x10
  #define LSR_THE		 0x20
  #define LSR_TEMT		 0x40
  #define LSR_FIE		 0x80

/*---------------------------------------------------------------------------+
| Modem status Register.
+---------------------------------------------------------------------------*/
  unsigned char MSR;	/* 0x06 */
  #define UART_MSR_DCTS		 0x01
  #define UART_MSR_DDSR		 0x02
  #define UART_MSR_TERI		 0x04
  #define UART_MSR_DDCD		 0x08
  #define UART_MSR_CTS		 0x10
  #define UART_MSR_DSR		 0x20
  #define UART_MSR_RI		 0x40
  #define UART_MSR_CD		 0x80

/*---------------------------------------------------------------------------+
| Scratch pad Register.
+---------------------------------------------------------------------------*/
  unsigned char SCR;	/* 0x07 */
};


#define USE_UART 0 /* 0=UART0 1=UART1 */
#define UART_INTERNAL_CLOCK_DIVISOR 16

typedef volatile struct async *pasync;
static const pasync port = (pasync)(0xEF600300   + (USE_UART*0x100));	/* 0xEF600300 - port A,  0xEF600400 - port B */

static void *spittyp;         /* handle for termios */
int ppc403_spi_interrupt = 0; /* do not use interrupts... */

extern uint32_t bsp_serial_per_sec;
extern uint32_t bsp_serial_rate;
extern bool bsp_serial_external_clock;

static int spiBaudRound(double x)
{
  return (int)((int)((x-(int)x)*1000)>500 ? x+1 : x);
}

void
spiBaudSet(uint32_t   baudrate)
{
  uint32_t   tmp;

  tmp = spiBaudRound( (double)bsp_serial_per_sec / (baudrate * 16) );

  port->LCR = port->LCR | LCR_DL;

  port->DLL = tmp & 0xff;
  port->DLM = tmp >> 8;

  port->LCR = port->LCR & ~LCR_DL;
}
/*
 * Hardware-dependent portion of tcsetattr().
 */
static int
spiSetAttributes (int minor, const struct termios *t)
{
  int baud;

  /* FIXME: check c_cflag & CRTSCTS for hardware flowcontrol */
  /* FIXME: check and IMPLEMENT XON/XOFF                     */
  switch (t->c_cflag & CBAUD) {
  default:	baud = -1;	break;
  case B50:	baud = 50;	break;
  case B75:	baud = 75;	break;
  case B110:	baud = 110;	break;
  case B134:	baud = 134;	break;
  case B150:	baud = 150;	break;
  case B200:	baud = 200;	break;
  case B300:	baud = 300;	break;
  case B600:	baud = 600;	break;
  case B1200:	baud = 1200;	break;
  case B1800:	baud = 1800;	break;
  case B2400:	baud = 2400;	break;
  case B4800:	baud = 4800;	break;
  case B9600:	baud = 9600;	break;
  case B19200:	baud = 19200;	break;
  case B38400:	baud = 38400;	break;
  case B57600:	baud = 57600;	break;
  case B115200:	baud = 115200;	break;
  case B230400:	baud = 230400;	break;
  case B460800:	baud = 460800;	break;
  }
  if (baud > 0) {
    spiBaudSet(baud);
  }
  return 0;
}

static int
spiPollRead (int minor)
{

  /* Wait for character */
  while ((port->LSR & LSR_RSR)==0);

  return port->RBR;
}


static ssize_t
spiPollWrite(int minor, const char *buf, size_t len)
{

  while (len-- > 0) {
    while (!(port->LSR & LSR_THE));
    port->THR = *buf++;
  }
  return 0;
}

/*
 * enable/disable RTS line to start/stop remote transmitter
 */
static int
spiStartRemoteTx (int minor)
{
/* Not implemented !
  rtems_interrupt_level level;

  rtems_interrupt_disable (level);
  port->SPCTL |= CRRts;            activate RTS
  rtems_interrupt_enable (level);
*/
  return 0;
}

static int
spiStopRemoteTx (int minor)
{
/* Not implemented !
  rtems_interrupt_level level;

  rtems_interrupt_disable (level);
  port->SPCTL &= ~CRRts;            deactivate RTS
  rtems_interrupt_enable (level);
*/
  return 0;
}

static ssize_t InterruptWrite (int minor, const char *buf, size_t len)
{
  port->IER |= IER_XMT;     /* always enable tx interrupt */
  port->THR = *buf; 	    /* write char to send         */
  return 0;
}

static rtems_isr serial_ISR(rtems_vector_number v)
{
  unsigned char _isr;
  char ch;
  int res;

  _isr=port->ISR & 0x0E;

   if ((_isr == ISR_Rx) || (_isr==ISR_RxTO)) {
    	ch = port->RBR;
    	rtems_termios_enqueue_raw_characters (spittyp,&ch,1);
   }

   if (_isr == ISR_Tx) {
	res = rtems_termios_dequeue_characters (spittyp,1);
	if (res==0) {
		port->IER &= ~IER_XMT;
		}

   }
}


/*
 *
 * deinit SPI
 *
 */
void
spiDeInit(void)
{
  /*
   * disable interrupts for serial port
   * set it to state to work with polling boot monitor, if any...
   */


  /* set up baud rate to original state */
  spiBaudSet(bsp_serial_rate);

  port->IER = 0;

}

/*
 *
 * init SPI
 *
 */
rtems_status_code
spiInitialize(void)
{
  register unsigned tmp;
  rtems_isr_entry previous_isr; /* this is a dummy */
  unsigned char _ier;

  /*
   * Initialise the serial port
   */

  /*
   * Select clock source and set uart internal clock divisor
   */

  __asm__ volatile ("mfdcr %0, 0x0b1" : "=r" (tmp)); /* CPC_CR0 0x0b1 */

  /* UART0 bit 24 0x80, UART1 bit 25 0x40 */
  tmp |= (bsp_serial_external_clock ?  (USE_UART ? 0x40 : 0x80) : 0);

  tmp |= (bsp_serial_external_clock ?  0: ((UART_INTERNAL_CLOCK_DIVISOR -1) << 1));

  __asm__ volatile ("mtdcr 0x0b1, %0" : "=r" (tmp) : "0" (tmp)); /* CPC_CR0 0x0b1*/

  /* Disable port interrupts while changing hardware */
  _ier = port->IER;
  port->IER = 0;

  /* set up port control: 8 bit,1 stop,no parity */
  port->LCR = LCR_WL8 | LCR_SB1 | LCR_PN;

  /* set up baud rate */
  spiBaudSet(bsp_serial_rate);

  if (ppc403_spi_interrupt) {

    /* add rx/tx isr to vector table */
    if (USE_UART==0)
	ictrl_set_vector(serial_ISR,PPC_IRQ_EXT_UART0,&previous_isr);
    else
	ictrl_set_vector(serial_ISR,PPC_IRQ_EXT_UART1,&previous_isr);

    /* Enable and clear FIFO */
    port->FCR = FCR_FE | FCR_CRF | FCR_CTF | FCR_RT8;

    /* Enable recive interrupts, don't enable TxInt yet */
    port->IER=IER_RCV;
  }
  else {
    port->IER=_ier;
  }

  atexit(spiDeInit);

  return RTEMS_SUCCESSFUL;
}

/*
 ***************
 * BOILERPLATE *
 ***************
 */

/*  console_initialize
 *
 *  This routine initializes the console IO driver.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;

  /*
   * Set up TERMIOS
   */
  rtems_termios_initialize ();

  /*
   * Do device-specific initialization
   */
  spiInitialize ();

  /*
   * Register the device
   */
  status = rtems_io_register_name ("/dev/console", major, 0);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
  return RTEMS_SUCCESSFUL;
}


/*
 *  Open entry point
 */

rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code sc;
  static const rtems_termios_callbacks intrCallbacks = {
    NULL,		/* firstOpen */
    NULL,		/* lastClose */
    NULL,	        /* pollRead */
    InterruptWrite,	/* write */
    spiSetAttributes,   /* setAttributes */
    spiStopRemoteTx,	/* stopRemoteTx */
    spiStartRemoteTx,	/* startRemoteTx */
    1			/* outputUsesInterrupts */
  };

  static const rtems_termios_callbacks pollCallbacks = {
    NULL,		/* firstOpen */
    NULL,		/* lastClose */
    spiPollRead,	/* pollRead */
    spiPollWrite,	/* write */
    spiSetAttributes,	/* setAttributes */
    spiStopRemoteTx,	/* stopRemoteTx */
    spiStartRemoteTx,	/* startRemoteTx */
    0			/* outputUsesInterrupts */
  };

  if (ppc403_spi_interrupt) {
    rtems_libio_open_close_args_t *args = arg;
    sc = rtems_termios_open (major, minor, arg, &intrCallbacks);
    spittyp = args->iop->data1;
  }
  else {
    sc = rtems_termios_open (major, minor, arg, &pollCallbacks);
  }
  return sc;
}

/*
 *  Close entry point
 */

rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_close (arg);
}

/*
 * read bytes from the serial port. We only have stdin.
 */

rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_read (arg);
}

/*
 * write bytes to the serial port. Stdout and stderr are the same.
 */

rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write (arg);
}

/*
 *  IO Control entry point
 */

rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl (arg);
}
