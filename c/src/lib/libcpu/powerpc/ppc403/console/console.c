/*
 *  This file contains the PowerPC 403GA console IO package.
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
 */

#define NO_BSP_INIT

#include <rtems.h>
#include <rtems/libio.h>
#include "../irq/ictrl.h"
#include <stdlib.h>                                     /* for atexit() */

struct async {
/*---------------------------------------------------------------------------+
| Line Status Register.
+---------------------------------------------------------------------------*/
  unsigned char SPLS;
  unsigned char SPLSset;
#define LSRDataReady             0x80
#define LSRFramingError          0x40
#define LSROverrunError          0x20
#define LSRParityError           0x10
#define LSRBreakInterrupt        0x08
#define LSRTxHoldEmpty           0x04
#define LSRTxShiftEmpty          0x02

/*---------------------------------------------------------------------------+
| Handshake Status Register.
+---------------------------------------------------------------------------*/
  unsigned char SPHS;
  unsigned char SPHSset;
#define HSRDsr                   0x80
#define HSRCts                   0x40

/*---------------------------------------------------------------------------+
| Baud rate divisor registers
+---------------------------------------------------------------------------*/
  unsigned char BRDH;
  unsigned char BRDL;

/*---------------------------------------------------------------------------+
| Control Register.
+---------------------------------------------------------------------------*/
  unsigned char SPCTL;
#define CRNormal		 0x00
#define CRLoopback		 0x40
#define CRAutoEcho		 0x80
#define CRDtr                    0x20
#define CRRts                    0x10
#define CRWordLength7            0x00
#define CRWordLength8            0x08
#define CRParityDisable          0x00
#define CRParityEnable           0x04
#define CREvenParity             0x00
#define CROddParity	         0x02
#define CRStopBitsOne            0x00
#define CRStopBitsTwo            0x01
#define CRDisableDtrRts	         0x00

/*--------------------------------------------------------------------------+
| Receiver Command Register.
+--------------------------------------------------------------------------*/
  unsigned char SPRC;
#define RCRDisable	         0x00
#define RCREnable		 0x80
#define RCRIntDisable	         0x00
#define RCRIntEnabled	         0x20
#define RCRDMACh2		 0x40
#define RCRDMACh3	         0x60
#define RCRErrorInt	         0x10
#define RCRPauseEnable	         0x08

/*--------------------------------------------------------------------------+
| Transmitter Command Register.
+--------------------------------------------------------------------------*/
    unsigned char SPTC;
#define TCRDisable	         0x00
#define TCREnable		 0x80
#define TCRIntDisable	         0x00
#define TCRIntEnabled 	         0x20
#define TCRDMACh2		 0x40
#define TCRDMACh3	         0x60
#define TCRTxEmpty		 0x10
#define TCRErrorInt	         0x08
#define TCRStopPause	         0x04
#define TCRBreakGen	         0x02

/*--------------------------------------------------------------------------+
| Miscellanies defines.
+--------------------------------------------------------------------------*/
  unsigned char SPTB;
#define SPRB	SPTB
};

typedef volatile struct async *pasync;
static const pasync port = (pasync)0x40000000;

static void *spittyp;         /* handle for termios */
int ppc403_spi_interrupt = 1; /* use interrupts... */

/*
 * Rx Interrupt handler
 */
static rtems_isr
spiRxInterruptHandler (rtems_vector_number v)
{
  char ch;

  /* clear any receive errors (errors are ignored now) */
  port->SPLS = (LSRFramingError | LSROverrunError |
                LSRParityError  | LSRBreakInterrupt);
  /*
   * Buffer received?
   */
  if (port->SPLS & LSRDataReady) {
    ch = port->SPRB; /* read receive buffer */
    rtems_termios_enqueue_raw_characters (spittyp,&ch,1);
  }
}

/*
 * Tx Interrupt handler
 */
static rtems_isr
spiTxInterruptHandler (rtems_vector_number v)
{
  /*
   * char transmitted?
   */
  if (0 != (port->SPLS & LSRTxHoldEmpty)) { /* must always be true!! */
    port->SPTC &= ~TCRIntEnabled;           /* stop irqs for now...  */
                                            /* and call termios...   */
    rtems_termios_dequeue_characters (spittyp,1);
  }
}

/*
 * enable/disable RTS line to start/stop remote transmitter
 */
static int
spiStartRemoteTx (int minor)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable (level);
  port->SPCTL |= CRRts;           /* activate RTS  */
  rtems_interrupt_enable (level);
  return 0;
}

static int
spiStopRemoteTx (int minor)
{
  rtems_interrupt_level level;

  rtems_interrupt_disable (level);
  port->SPCTL &= ~CRRts;           /* deactivate RTS  */
  rtems_interrupt_enable (level);
  return 0;
}

void
spiBaudSet(uint32_t   baudrate)
{
  uint32_t   tmp;
  extern uint32_t bsp_serial_per_sec;
  tmp = bsp_serial_per_sec / baudrate;
  tmp = ((tmp) >> 4) - 1;
  port->BRDL = tmp & 0xff;
  port->BRDH = tmp >> 8;

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
  unsigned char status;

  while (0 == ((status = port->SPLS) & LSRDataReady)) {
    /* Clean any dodgy status */
    if ((status & (LSRFramingError | LSROverrunError | LSRParityError |
		   LSRBreakInterrupt)) != 0) {
      port->SPLS = (LSRFramingError | LSROverrunError | LSRParityError |
		    LSRBreakInterrupt);
    }
  }
  return port->SPRB;
}

static int
spiInterruptWrite (int minor, const char *buf, int len)
{
  port->SPTB = *buf;           /* write char to send         */
  port->SPTC |= TCRIntEnabled; /* always enable tx interrupt */
  return 0;
}

static int
spiPollWrite(int minor,const char *buf,int len)
{
  unsigned char status;

  while (len-- > 0) {
    do {
      if (port->SPHS) {
	port->SPHS = (HSRDsr | HSRCts);
      }
      status = port->SPLS;
    } while (0 == (status & LSRTxHoldEmpty));
    port->SPTB = *buf++;
  }
  return 0;
}

/*
 *
 * deinit SPI
 *
 */
void
spiDeInit(void)
{
  extern uint32_t bsp_serial_rate;
  /*
   * disable interrupts for serial port
   * set it to state to work with polling boot monitor, if any...
   */

  /* set up baud rate to original state */
  spiBaudSet(bsp_serial_rate);

  /* clear any receive (error) status */
  port->SPLS = (LSRDataReady   | LSRFramingError | LSROverrunError |
		LSRParityError | LSRBreakInterrupt);

  /* set up port control: DTR/RTS active,8 bit,1 stop,no parity */
  port->SPCTL = (CRNormal |
		 CRDtr | CRRts |
		 CRWordLength8 | CRParityDisable | CRStopBitsOne);

  /* clear handshake status bits */
  port->SPHS = (HSRDsr | HSRCts);

  /* enable receiver/transmitter, no interrupts */
  port->SPRC = (RCREnable | RCRIntDisable);
  port->SPTC = (TCREnable | TCRIntDisable);

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
  extern bool bsp_serial_external_clock;
  extern uint32_t bsp_serial_rate;

  /*
   * Initialise the serial port
   */

  /*
   * select RTS/CTS hardware handshake lines,
   * select clock source
   */
  __asm__ volatile ("mfdcr %0, 0xa0" : "=r" (tmp)); /* IOCR */

  tmp &= ~3;
  tmp |= (bsp_serial_external_clock ? 2 : 0) | 1;

  __asm__ volatile ("mtdcr 0xa0, %0" : "=r" (tmp) : "0" (tmp)); /* IOCR */

  /* clear any receive (error) status */
  port->SPLS = (LSRDataReady   | LSRFramingError | LSROverrunError |
		LSRParityError | LSRBreakInterrupt);

  /* set up baud rate */
  spiBaudSet(bsp_serial_rate);

  /* set up port control: DTR/RTS active,8 bit,1 stop,no parity */
  port->SPCTL = (CRNormal |
		 CRDtr | CRRts |
		 CRWordLength8 | CRParityDisable | CRStopBitsOne);

  /* clear handshake status bits */
  port->SPHS = (HSRDsr | HSRCts);

  if (ppc403_spi_interrupt) {
    /* add rx/tx isr to vector table */
    ictrl_set_vector(spiRxInterruptHandler,
		     PPC_IRQ_EXT_SPIR,
		     &previous_isr);

    ictrl_set_vector(spiTxInterruptHandler,
		     PPC_IRQ_EXT_SPIT,
		     &previous_isr);

    port->SPRC = (RCREnable | RCRIntEnabled | RCRErrorInt);
    port->SPTC = (TCREnable | TCRIntDisable); /* don't enable TxInt yet */
  }
  else {
    /* enable receiver/transmitter, no interrupts */
    port->SPRC = (RCREnable | RCRIntDisable);
    port->SPTC = (TCREnable | TCRIntDisable);
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
    spiInterruptWrite,	/* write */
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
