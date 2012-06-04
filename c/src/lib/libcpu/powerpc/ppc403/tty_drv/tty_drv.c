/*
 * This file contains the PowerPC 405GP tty driver.
 *
 * Derived from /c/src/lib/libbsp/i386/shared/comm/tty_drv.c
 *
 * Modifications to PPC405GP by Dennis Ehlin
 *
 */

#define NO_BSP_INIT

#include <stdio.h>
#include <rtems/termiostypes.h>
#include <termios.h>
#include <assert.h>
#include <rtems.h>
#include <rtems/libio.h>
#include "../irq/ictrl.h"
#include <stdlib.h>                                     /* for atexit() */

extern uint32_t bsp_serial_per_sec;
extern bool bsp_serial_external_clock;
extern bool bsp_serial_cts_rts;
extern bool bsp_serial_xon_xoff;
extern uint32_t bsp_serial_rate;

struct ttyasync {
/*---------------------------------------------------------------------------+
| Data Register.
+---------------------------------------------------------------------------*/
  unsigned char RBR;	/* 0x00 */
  #define THR   RBR
/*---------------------------------------------------------------------------+
| Interrupt registers
+---------------------------------------------------------------------------*/
  unsigned char IER;    /* Interrupt Enable Register 0x01 */
  #define IER_RCV		 0x01
  #define IER_XMT		 0x02
  #define IER_LS		 0x04
  #define IER_MS		 0x08

  unsigned char ISR;    /* Interrupt Status Register 0x02 */
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


#define TTY0_USE_UART 1 /* 0=UART0 1=UART1 */
#define TTY0_UART_INTERNAL_CLOCK_DIVISOR 16
#define TTY0_USE_INTERRUPT


typedef volatile struct ttyasync *tty0pasync;
static const tty0pasync tty0port = (tty0pasync)(0xEF600300   + (TTY0_USE_UART*0x100));	/* 0xEF600300 - port A,  0xEF600400 - port B */

static void *tty0ttyp;         /* handle for termios */


int tty0_round(double x)
{
  return (int)((int)((x-(int)x)*1000)>500 ? x+1 : x);
}

void
tty0BaudSet(uint32_t   baudrate)
{
  uint32_t   tmp;

  tmp = tty0_round( (double)bsp_serial_per_sec / (baudrate * 16) );

  tty0port->LCR = tty0port->LCR | LCR_DL;

  tty0port->DLL = tmp & 0xff;
  tty0port->DLM = tmp >> 8;

  tty0port->LCR = tty0port->LCR & ~LCR_DL;
}
/*
 * Hardware-dependent portion of tcsetattr().
 */
static int
tty0SetAttributes (int minor, const struct termios *t)
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
    tty0BaudSet(baud);
  }
  return 0;
}

#ifndef TTY0_USE_INTERRUPT
static int
tty0PollRead (int minor)
{

  /* Wait for character */
  while ((tty0port->LSR & LSR_RSR)==0);

  return tty0port->RBR;
}


static ssize_t
tty0PollWrite(int minor, const char *buf, size_t len)
{

  while (len-- > 0) {
    while (!(tty0port->LSR & LSR_THE));
    tty0port->THR = *buf++;
  }
  return 0;
}
#endif

/* ================ Termios support  =================*/

static ssize_t tty0InterruptWrite (int minor, const char *buf, size_t len)
{

  if(len <= 0)
    {
      return 0;
    }

  /* Write character */

  tty0port->THR = (*buf &0xff);
  tty0port->IER |= IER_XMT;     /* always enable tx interrupt */

  return 0;

}

static rtems_isr tty0serial_ISR(rtems_vector_number v)
{
  char buf[128];
  int      off, ret, vect;

  off = 0;

  for(;;)
    {
	  vect = tty0port->ISR & 0x0f;
	  if(vect & 1)
	  {
		  /* no more interrupts */
          if(off > 0) {
                  /* Update rx buffer */
  		  rtems_termios_enqueue_raw_characters(tty0ttyp, buf, off );

  	          tty0port->IER |= IER_RCV;     /* always enable rx interrupt */
		 /*rtems_termios_rxirq_occured(tty0ttyp);*/
	  }
	    return;
	  }

	vect = vect & 0xe; /*mask out all except interrupt pending*/

    switch(vect)
	{

	case ISR_Tx :
	  /*
	   * TX holding empty: we have to disable these interrupts
	   * if there is nothing more to send.
	   */

	  /* If nothing else to send disable interrupts */
	  ret = rtems_termios_dequeue_characters(tty0ttyp, 1);

          if ( ret == 0 ) {
   	    tty0port->IER &= ~IER_XMT;
          }

	  break;
	case ISR_RxTO:
	case ISR_Rx :

              /* disable interrupts and notify termios */
	       tty0port->IER &= ~IER_RCV;

	      /* read all bytes in fifo*/
  	       while (( off < sizeof(buf) ) && (  tty0port->LSR & LSR_RSR ))
			  {
				buf[off++] = tty0port->RBR;
			  }

	  break;
	case ISR_LS:
	  /* RX error: eat character */
	  /* printk("********* Error **********\n"); */
 	  break;
	default:
	  /* Should not happen */
	  /* printk("error vect=%x",vect); */
	  return;
	}
    }

}


/*
 *
 * deinit TTY0
 *
 */
void
tty0DeInit(void)
{
  /*
   * disable interrupts for serial tty0port
   * set it to state to work with polling boot monitor, if any...
   */

  /* set up baud rate to original state */
  tty0BaudSet(bsp_serial_rate);

  tty0port->IER = 0;

}

/*
 *
 * init SPI
 *
 */
rtems_status_code
tty0Initialize(void)
{
  register unsigned tmp;
  rtems_isr_entry previous_isr; /* this is a dummy */
  unsigned char _ier;
  unsigned char _tmp;

  /*
   * Initialise the serial tty0port
   */

  /*
   * Select clock source and set uart internal clock divisor
   */

  __asm__ volatile ("mfdcr %0, 0x0b1" : "=r" (tmp)); /* CPC_CR0 0x0b1 */

  /* UART0 bit 24 0x80, UART1 bit 25 0x40 */
  tmp |= (bsp_serial_external_clock ?  (TTY0_USE_UART ? 0x40 : 0x80) : 0);

  tmp |= (bsp_serial_external_clock ?  0: ((TTY0_UART_INTERNAL_CLOCK_DIVISOR -1) << 1));

  __asm__ volatile ("mtdcr 0x0b1, %0" : "=r" (tmp) : "0" (tmp)); /* CPC_CR0 0x0b1*/

  /* Disable tty0port interrupts while changing hardware */
  _ier = tty0port->IER;
  tty0port->IER = 0;

  /* set up tty0port control: 8 bit,1 stop,no parity */
  tty0port->LCR = LCR_WL8 | LCR_SB1 | LCR_PN;

  /* set up baud rate */
  tty0BaudSet(bsp_serial_rate);


#ifdef TTY0_USE_INTERRUPT

    /* add rx/tx isr to vector table */

    if (TTY0_USE_UART==0)
	ictrl_set_vector(tty0serial_ISR,PPC_IRQ_EXT_UART0,&previous_isr);
    else
	ictrl_set_vector(tty0serial_ISR,PPC_IRQ_EXT_UART1,&previous_isr);

    /* Enable and clear FIFO */
    tty0port->FCR = FCR_FE | FCR_CRF | FCR_CTF | FCR_RT14;

    /* Read status to clear them */
    _tmp = tty0port->LSR;
    _tmp = tty0port->RBR;
    _tmp = tty0port->MSR;

    /* Enable recive interrupts, don't enable TxInt yet */
    tty0port->IER=IER_RCV;

#else

    tty0port->IER=_ier;

#endif

  atexit(tty0DeInit);

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

rtems_device_driver tty0_initialize(
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

  /*tty0Initialize ();   Moved this to open instead */

  /*
   * Register the device
   */
  status = rtems_io_register_name ("/dev/ttyS0", major, 0);
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred (status);
  return RTEMS_SUCCESSFUL;
}


/*
 *  Open entry point
 */

rtems_device_driver tty0_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code sc;

#ifdef TTY0_USE_INTERRUPT

  static const rtems_termios_callbacks intrCallbacks = {
    NULL,		/* firstOpen */
    NULL,		/* lastClose */
    NULL, 		/* pollRead */
    tty0InterruptWrite,	/* write */
    tty0SetAttributes,  /* setAttributes */
    NULL,	        /* stopRemoteTx */
    NULL,	        /* startRemoteTx */
    TERMIOS_TASK_DRIVEN	/* outputUsesInterrupts */
  };
  rtems_libio_open_close_args_t *args = arg;

    tty0Initialize (); 	/* Initalize hardware */

    sc = rtems_termios_open (major, minor, arg, &intrCallbacks);
    tty0ttyp = args->iop->data1;

#else

  static const rtems_termios_callbacks pollCallbacks = {
    NULL,		/* firstOpen */
    NULL,		/* lastClose */
    tty0PollRead,	/* pollRead */
    tty0PollWrite,	/* write */
    tty0SetAttributes,	/* setAttributes */
    NULL,		/* stopRemoteTx */
    NULL,		/* startRemoteTx */
    0			/* outputUsesInterrupts */
  };

    tty0Initialize (); 	/* Initalize hardware */

    sc = rtems_termios_open (major, minor, arg, &pollCallbacks);

#endif

    return sc;
}

/*
 *  Close entry point
 */

rtems_device_driver tty0_close(
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

rtems_device_driver tty0_read(
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

rtems_device_driver tty0_write(
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

rtems_device_driver tty0_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl (arg);
}
