/*
 * This software is Copyright (C) 1998 by T.sqware - all rights limited
 * It is provided in to the public domain "as is", can be freely modified
 * as far as this copyight notice is kept unchanged, but does not imply
 * an endorsement by T.sqware of the product in which it is included.
 */

#include <stdio.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/uart.h>
#include <rtems/libio.h>
#include <rtems/bspIo.h>
#include <rtems/termiostypes.h>
#include <termios.h>
#include <assert.h>

/*
 * Basic 16552 driver
 */

struct uart_data
{
  unsigned long       ioBase;
  int                 irq;
  int                 hwFlow;
  int                 baud;
  BSP_UartBreakCbRec  breakCallback;
  int                 ioMode;
};

/*
 * Initialization of BSP specific data.
 * The constants are pulled in from a BSP
 * specific file, whereas all of the code
 * in this file is generic and makes no
 * assumptions about addresses, irq vectors
 * etc...
 */

#define UART_UNSUPP ((unsigned long)(-1))

static struct uart_data uart_data[2] = {
	{
#ifdef	BSP_UART_IOBASE_COM1
		BSP_UART_IOBASE_COM1,
		BSP_UART_COM1_IRQ,
#else
		UART_UNSUPP,
		-1,
#endif
	},
	{
#ifdef	BSP_UART_IOBASE_COM2
		BSP_UART_IOBASE_COM2,
		BSP_UART_COM2_IRQ,
#else
		UART_UNSUPP,
		-1,
#endif
	},
};

#define MAX_UARTS (sizeof(uart_data)/sizeof(uart_data[0]))
#define SANITY_CHECK(uart) \
  assert( MAX_UARTS > (unsigned)(uart) && uart_data[(uart)].ioBase != UART_UNSUPP )
/*
 * Macros to read/write register of uart, if configuration is
 * different just rewrite these macros
 */

static inline unsigned char
uread(int uart, unsigned int reg)
{
	return in_8((unsigned char*)(uart_data[uart].ioBase + reg));
}

static inline void
uwrite(int uart, int reg, unsigned int val)
{
	out_8((unsigned char*)(uart_data[uart].ioBase + reg), val);
}


#ifdef UARTDEBUG
    static void
uartError(int uart, void *termiosPrivate)
{
  unsigned char uartStatus, dummy;
  BSP_UartBreakCbProc		h;

  uartStatus = uread(uart, LSR);
  dummy = uread(uart, RBR);

  if (uartStatus & OE)
    printk("********* Over run Error **********\n");
  if (uartStatus & PE)
    printk("********* Parity Error   **********\n");
  if (uartStatus & FE)
    printk("********* Framing Error  **********\n");
  if (uartStatus & BI) {
    printk("********* BREAK INTERRUPT *********\n");
	if ((h=uart_data[uart].breakCallback.handler))
		h(uart,
		  (dummy<<8)|uartStatus,
		  termiosPrivate,
		  uart_data[uart].breakCallback.private);

  }
  if (uartStatus & ERFIFO)
    printk("********* Error receive Fifo **********\n");

}
#else
inline void uartError(int uart, void *termiosPrivate)
{
  unsigned char uartStatus,dummy;
  BSP_UartBreakCbProc		h;

  uartStatus = uread(uart, LSR);
  dummy		 = uread(uart, RBR);
  if ((uartStatus & BI) && (h=uart_data[uart].breakCallback.handler))
		h(uart,
		  (dummy<<8)|uartStatus,
		  termiosPrivate,
		  uart_data[uart].breakCallback.private);
}
#endif

/*
 * Uart initialization, it is hardcoded to 8 bit, no parity,
 * one stop bit, FIFO, things to be changed
 * are baud rate and nad hw flow control,
 * and longest rx fifo setting
 */
void
BSP_uart_init(int uart, int baud, int hwFlow)
{
  unsigned char tmp;

  /* Sanity check */
  SANITY_CHECK(uart);

  /* Make sure any printk activity drains before
   * re-initializing.
   */
  while ( ! (uread(uart, LSR) & TEMT) )
	;

  switch(baud)
    {
    case 50:
    case 75:
    case 110:
    case 134:
    case 300:
    case 600:
    case 1200:
    case 2400:
    case 9600:
    case 19200:
    case 38400:
    case 57600:
    case 115200:
      break;
    default:
      assert(0);
      return;
    }

  /* Set DLAB bit to 1 */
  uwrite(uart, LCR, DLAB);

  if ( (int)BSPBaseBaud <= 0 ) {
  	/* Use current divisor assuming BSPBaseBaud gives us the current speed */
	BSPBaseBaud  = BSPBaseBaud ? -BSPBaseBaud : 9600;
	BSPBaseBaud *= ((uread(uart, DLM) << 8) | uread(uart, DLL));
  }

  /* Set baud rate */
  uwrite(uart, DLL,  (BSPBaseBaud/baud) & 0xff);
  uwrite(uart, DLM,  ((BSPBaseBaud/baud) >> 8) & 0xff);

  /* 8-bit, no parity , 1 stop */
  uwrite(uart, LCR, CHR_8_BITS);

  /* Set DTR, RTS and OUT2 high */
  uwrite(uart, MCR, DTR | RTS | OUT_2);

  /* Enable FIFO */
  uwrite(uart, FCR, FIFO_EN | XMIT_RESET | RCV_RESET | RECEIVE_FIFO_TRIGGER12);

  /* Disable Interrupts */
  uwrite(uart, IER, 0);

  /* Read status to clear them */
  tmp = uread(uart, LSR);
  tmp = uread(uart, RBR);
  tmp = uread(uart, MSR);

  /* Remember state */
  uart_data[uart].hwFlow     = hwFlow;
  uart_data[uart].baud       = baud;
  return;
}

/*
 * Set baud
 */
void
BSP_uart_set_baud(int uart, int baud)
{
  unsigned char mcr, ier;

  /* Sanity check */
  SANITY_CHECK(uart);

  /*
   * This function may be called whenever TERMIOS parameters
   * are changed, so we have to make sure that baud change is
   * indeed required.
   */

  if(baud == uart_data[uart].baud)
    {
      return;
    }

  mcr = uread(uart, MCR);
  ier = uread(uart, IER);

  BSP_uart_init(uart, baud, uart_data[uart].hwFlow);

  uwrite(uart, MCR, mcr);
  uwrite(uart, IER, ier);

  return;
}

/*
 * Enable/disable interrupts
 */
void
BSP_uart_intr_ctrl(int uart, int cmd)
{

  SANITY_CHECK(uart);

  switch(cmd)
    {
    case BSP_UART_INTR_CTRL_DISABLE:
      uwrite(uart, IER, INTERRUPT_DISABLE);
      break;
    case BSP_UART_INTR_CTRL_ENABLE:
      if(uart_data[uart].hwFlow)
	{
	  uwrite(uart, IER,
		 (RECEIVE_ENABLE  |
		  TRANSMIT_ENABLE |
		  RECEIVER_LINE_ST_ENABLE |
		  MODEM_ENABLE
		 )
		);
	}
      else
	{
	  uwrite(uart, IER,
		 (RECEIVE_ENABLE  |
		  TRANSMIT_ENABLE |
		  RECEIVER_LINE_ST_ENABLE
		 )
		);
	}
      break;
    case BSP_UART_INTR_CTRL_TERMIOS:
      if(uart_data[uart].hwFlow)
	{
	  uwrite(uart, IER,
		 (RECEIVE_ENABLE  |
		  RECEIVER_LINE_ST_ENABLE |
		  MODEM_ENABLE
		 )
		);
	}
      else
	{
	  uwrite(uart, IER,
		 (RECEIVE_ENABLE  |
		  RECEIVER_LINE_ST_ENABLE
		 )
		);
	}
      break;
    case BSP_UART_INTR_CTRL_GDB:
      uwrite(uart, IER, RECEIVE_ENABLE);
      break;
    default:
      assert(0);
      break;
    }

  return;
}

void
BSP_uart_throttle(int uart)
{
  unsigned int mcr;

  SANITY_CHECK(uart);

  if(!uart_data[uart].hwFlow)
    {
      /* Should not happen */
      assert(0);
      return;
    }
  mcr = uread (uart, MCR);
  /* RTS down */
  mcr &= ~RTS;
  uwrite(uart, MCR, mcr);

  return;
}

void
BSP_uart_unthrottle(int uart)
{
  unsigned int mcr;

  SANITY_CHECK(uart);

  if(!uart_data[uart].hwFlow)
    {
      /* Should not happen */
      assert(0);
      return;
    }
  mcr = uread (uart, MCR);
  /* RTS up */
  mcr |= RTS;
  uwrite(uart, MCR, mcr);

  return;
}

/*
 * Status function, -1 if error
 * detected, 0 if no received chars available,
 * 1 if received char available, 2 if break
 * is detected, it will eat break and error
 * chars. It ignores overruns - we cannot do
 * anything about - it execpt count statistics
 * and we are not counting it.
 */
int
BSP_uart_polled_status(int uart)
{
  unsigned char val;

  SANITY_CHECK(uart);

  val = uread(uart, LSR);

  if(val & BI)
    {
      /* BREAK found, eat character */
      uread(uart, RBR);
      return BSP_UART_STATUS_BREAK;
    }

  if((val & (DR | OE | FE)) ==  1)
    {
      /* No error, character present */
      return BSP_UART_STATUS_CHAR;
    }

  if((val & (DR | OE | FE)) == 0)
    {
      /* Nothing */
      return BSP_UART_STATUS_NOCHAR;
    }

  /*
   * Framing or parity error
   * eat character
   */
  uread(uart, RBR);

  return BSP_UART_STATUS_ERROR;
}

/*
 * Polled mode write function
 */
void
BSP_uart_polled_write(int uart, int val)
{
  unsigned char val1;

  /* Sanity check */
  SANITY_CHECK(uart);

  for(;;)
    {
      if((val1=uread(uart, LSR)) & THRE)
	{
	  break;
	}
    }

  if(uart_data[uart].hwFlow)
    {
      for(;;)
	{
	  if(uread(uart, MSR) & CTS)
	    {
	      break;
	    }
	}
    }

  uwrite(uart, THR, val & 0xff);

  return;
}

void
BSP_output_char_via_serial(const char val)
{
  BSP_uart_polled_write(BSPConsolePort, val);
  if (val == '\n') BSP_uart_polled_write(BSPConsolePort,'\r');
}

/*
 * Polled mode read function
 */
int
BSP_uart_polled_read(int uart)
{
  unsigned char val;

  SANITY_CHECK(uart);

  for(;;)
    {
      if(uread(uart, LSR) & DR)
	{
	  break;
	}
    }

  val = uread(uart, RBR);

  return (int)(val & 0xff);
}

unsigned
BSP_poll_char_via_serial()
{
	return BSP_uart_polled_read(BSPConsolePort);
}

static void
uart_noop(const rtems_irq_connect_data *unused)
{
  return;
}

/* note that the IRQ names contain _ISA_ for legacy
 * reasons. They can be any interrupt, depending
 * on the particular BSP...
 */

static int
uart_isr_is_on(const rtems_irq_connect_data *irq)
{
  int uart;

  uart = (irq->name == BSP_UART_COM1_IRQ) ?
			BSP_UART_COM1 : BSP_UART_COM2;

  return uread(uart,IER);
}

static int
doit(int uart, rtems_irq_hdl handler, int (*p)(const rtems_irq_connect_data*))
{
	rtems_irq_connect_data d={0};
	d.name = uart_data[uart].irq;
	d.off  = d.on = uart_noop;
	d.isOn = uart_isr_is_on;
	d.hdl  = handler;
	return p(&d);
}

int
BSP_uart_install_isr(int uart, rtems_irq_hdl handler)
{
/* Using shared interrupts by default might break things.. the
 * shared IRQ installer uses malloc() and if a BSP had called this
 * during early init it might not work...
 */
#ifdef BSP_UART_USE_SHARED_IRQS
	return doit(uart, handler, BSP_install_rtems_shared_irq_handler);
#else
	return doit(uart, handler, BSP_install_rtems_irq_handler);
#endif
}

int
BSP_uart_remove_isr(int uart, rtems_irq_hdl handler)
{
	return doit(uart, handler, BSP_remove_rtems_irq_handler);
}

/* ================ Termios support  =================*/

static volatile int  termios_stopped_com[2]        = {0,0};
static volatile int  termios_tx_active_com[2]      = {0,0};
static void*	     termios_ttyp_com[2]           = {NULL,NULL};
static char          termios_tx_hold_com[2]        = {0,0};
static volatile char termios_tx_hold_valid_com[2]  = {0,0};

/*
 * Set channel parameters
 */
void
BSP_uart_termios_set(int uart, void *p)
{
  struct rtems_termios_tty *ttyp = p;
  unsigned char val;
  SANITY_CHECK(uart);

  if(uart_data[uart].hwFlow)
    {
      val = uread(uart, MSR);

      termios_stopped_com[uart]    = (val & CTS) ? 0 : 1;
    }
  else
  {
      termios_stopped_com[uart] = 0;
  }
  termios_tx_active_com[uart]      = 0;
  termios_ttyp_com[uart]           = ttyp;
  termios_tx_hold_com[uart]        = 0;
  termios_tx_hold_valid_com[uart]  = 0;

  uart_data[uart].ioMode           = ttyp->device.outputUsesInterrupts;

  return;
}

ssize_t
BSP_uart_termios_write_polled(int minor, const char *buf, size_t len)
{
  int uart=minor;	/* could differ, theoretically */
  int nwrite;
  const char *b = buf;

  assert(buf != NULL);

  for (nwrite=0 ; nwrite < len ; nwrite++) {
    BSP_uart_polled_write(uart, *b++);
  }
  return nwrite;
}

ssize_t
BSP_uart_termios_write_com(int minor, const char *buf, size_t len)
{
  int uart=minor;	/* could differ, theoretically */
  assert(buf != NULL);

  if(len <= 0)
    {
      return 0;
    }

  /* If the TX buffer is busy - something is royally screwed up */
  /*   assert((uread(BSP_UART_COM1, LSR) & THRE) != 0); */

  if(termios_stopped_com[uart])
    {
      /* CTS low */
      termios_tx_hold_com[uart]       = *buf;
      termios_tx_hold_valid_com[uart] = 1;
      return 0;
    }

  /* Write character */
  uwrite(uart, THR, *buf & 0xff);

  /* Enable interrupts if necessary */
  if(!termios_tx_active_com[uart] && uart_data[uart].hwFlow)
    {
      termios_tx_active_com[uart] = 1;
      uwrite(uart, IER,
	     (RECEIVE_ENABLE  |
	      TRANSMIT_ENABLE |
	      RECEIVER_LINE_ST_ENABLE |
	      MODEM_ENABLE
	     )
	    );
    }
  else if(!termios_tx_active_com[uart])
    {
      termios_tx_active_com[uart] = 1;
      uwrite(uart, IER,
	     (RECEIVE_ENABLE  |
	      TRANSMIT_ENABLE |
	      RECEIVER_LINE_ST_ENABLE
	     )
	    );
    }

  return 0;
}

int
BSP_uart_termios_read_com(int uart)
{
  int     off = (int)0;
  char    buf[40];
  rtems_interrupt_level l;

  /* read bytes */
  while (( off < sizeof(buf) ) && ( uread(uart, LSR) & DR )) {
    buf[off++] = uread(uart, RBR);
  }

  /* write out data */
  if ( off > 0 ) {
    rtems_termios_enqueue_raw_characters(termios_ttyp_com[uart], buf, off);
  }

  /* enable receive interrupts */
  rtems_interrupt_disable(l);
  uwrite(uart, IER, uread(uart, IER) | (RECEIVE_ENABLE | RECEIVER_LINE_ST_ENABLE));
  rtems_interrupt_enable(l);

  return ( EOF );
}

static void
BSP_uart_termios_isr_com(int uart)
{
  unsigned char buf[40];
  unsigned char val, ier;
  int      off, ret, vect;

  off = 0;

  for(;;)
    {
      vect = uread(uart, IIR) & 0xf;

      switch(vect)
	{
	case MODEM_STATUS :
	  val = uread(uart, MSR);
	  if(uart_data[uart].hwFlow)
	    {
	      if(val & CTS)
		{
		  /* CTS high */
		  termios_stopped_com[uart] = 0;
		  if(termios_tx_hold_valid_com[uart])
		    {
		      termios_tx_hold_valid_com[uart] = 0;
		      BSP_uart_termios_write_com(uart, &termios_tx_hold_com[uart],
						    1);
		    }
		}
	      else
		{
		  /* CTS low */
		  termios_stopped_com[uart] = 1;
		}
	    }
	  break;
	case NO_MORE_INTR :
	  /* No more interrupts */
	  if(off != 0)
	    {
	      /* Update rx buffer */
	      rtems_termios_enqueue_raw_characters(termios_ttyp_com[uart],
						   (char *)buf,
						   off);
	    }
	  return;
	case TRANSMITTER_HODING_REGISTER_EMPTY :
	  /*
	   * TX holding empty: we have to disable these interrupts
	   * if there is nothing more to send.
	   */

	  ret = rtems_termios_dequeue_characters(termios_ttyp_com[uart], 1);

	  /* If nothing else to send disable interrupts */
	  if(ret == 0 && uart_data[uart].hwFlow)
	    {
	      uwrite(uart, IER,
		     (RECEIVE_ENABLE  |
		      RECEIVER_LINE_ST_ENABLE |
		      MODEM_ENABLE
		     )
		    );
              termios_tx_active_com[uart] = 0;
	    }
	  else if(ret == 0)
	    {
	      uwrite(uart, IER,
		     (RECEIVE_ENABLE  |
		      RECEIVER_LINE_ST_ENABLE
		     )
		    );
              termios_tx_active_com[uart] = 0;
	    }
	  break;
	case RECEIVER_DATA_AVAIL :
	case CHARACTER_TIMEOUT_INDICATION:
	  if ( uart_data[uart].ioMode == TERMIOS_TASK_DRIVEN )
	    {
	      /* ensure interrupts are enabled */
	      if ( (ier = uread(uart,IER)) & RECEIVE_ENABLE )
	        {
	           /* disable interrupts and notify termios */
	           ier &= ~(RECEIVE_ENABLE | RECEIVER_LINE_ST_ENABLE);
	           uwrite(uart, IER, ier);
	           rtems_termios_rxirq_occured(termios_ttyp_com[uart]);
	        }
	    }
	  else
	    {
	      /* RX data ready */
	      assert(off < sizeof(buf));
	      while ( off < sizeof(buf) && ( DR & uread(uart, LSR) ) )
	        buf[off++] = uread(uart, RBR);
	    }
	  break;
	case RECEIVER_ERROR:
	  /* RX error: eat character */
	   uartError(uart, termios_ttyp_com[uart]);
	  break;
	default:
	  /* Should not happen */
	  assert(0);
	  return;
	}
    }
}

/*
 * XXX - Note that this can now be one isr with the uart
 *       passed as the parameter.
 */
void
BSP_uart_termios_isr_com1(void *unused)
{
	BSP_uart_termios_isr_com(BSP_UART_COM1);
}

void
BSP_uart_termios_isr_com2(void *unused)
{
	BSP_uart_termios_isr_com(BSP_UART_COM2);
}

/* retrieve 'break' handler info */
int
BSP_uart_get_break_cb(int uart, rtems_libio_ioctl_args_t *arg)
{
BSP_UartBreakCb	cb=arg->buffer;
unsigned long flags;
	SANITY_CHECK(uart);
	rtems_interrupt_disable(flags);
		*cb = uart_data[uart].breakCallback;
	rtems_interrupt_enable(flags);
	arg->ioctl_return=0;
	return RTEMS_SUCCESSFUL;
}

/* install 'break' handler */
int
BSP_uart_set_break_cb(int uart, rtems_libio_ioctl_args_t *arg)
{
BSP_UartBreakCb	cb=arg->buffer;
unsigned long flags;
	SANITY_CHECK(uart);
	rtems_interrupt_disable(flags);
		uart_data[uart].breakCallback = *cb;
	rtems_interrupt_enable(flags);
	arg->ioctl_return=0;
	return RTEMS_SUCCESSFUL;
}
