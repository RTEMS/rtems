/*
 * This software is Copyright (C) 1998 by T.sqware - all rights limited
 * It is provided in to the public domain "as is", can be freely modified
 * as far as this copyight notice is kept unchanged, but does not imply
 * an endorsement by T.sqware of the product in which it is included.
 */

#include <stdio.h>
#include <bsp.h>
#include <bsp/irq.h>
#include <uart.h>
#include <rtems/libio.h>
#include <rtems/termiostypes.h>
#include <termios.h>
#include <assert.h>

/*
 * Basic 16552 driver
 */

struct uart_data
{
  int ioMode;
  int hwFlow;
  unsigned int  ier;
  unsigned long baud;
  unsigned long databits;
  unsigned long parity;
  unsigned long stopbits;
};

static struct uart_data uart_data[2];

/*
 * Macros to read/write register of uart, if configuration is
 * different just rewrite these macros
 */

static inline unsigned char
uread(int uart, unsigned int reg)
{
  register unsigned char val;

  if (uart == 0) {
    inport_byte(COM1_BASE_IO+reg, val);
  } else {
    inport_byte(COM2_BASE_IO+reg, val);
  }

  return val;
}

static inline void
uwrite(int uart, int reg, unsigned int val)
{
  if (uart == 0) {
    outport_byte(COM1_BASE_IO+reg, val);
  } else {
    outport_byte(COM2_BASE_IO+reg, val);
  }
}

#ifdef UARTDEBUG
    static void
uartError(int uart)
{
  unsigned char uartStatus, dummy;

  uartStatus = uread(uart, LSR);
  dummy = uread(uart, RBR);

  if (uartStatus & OE)
    printk("********* Over run Error **********\n");
  if (uartStatus & PE)
    printk("********* Parity Error   **********\n");
  if (uartStatus & FE)
    printk("********* Framing Error  **********\n");
  if (uartStatus & BI)
    printk("********* Parity Error   **********\n");
  if (uartStatus & ERFIFO)
    printk("********* Error receive Fifo **********\n");

}
#else
inline void uartError(int uart)
{
  uread(uart, LSR);
  uread(uart, RBR);
}
#endif

/*
 * Uart initialization, it is hardcoded to 8 bit, no parity,
 * one stop bit, FIFO, things to be changed
 * are baud rate and nad hw flow control,
 * and longest rx fifo setting
 */
void
BSP_uart_init
(
  int uart,
  unsigned long baud,
  unsigned long databits,
  unsigned long parity,
  unsigned long stopbits,
  int hwFlow
)
{
  /* Sanity check */
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

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

  /* Set baud rate */
  uwrite(uart, DLL,  (BSPBaseBaud/baud) & 0xff);
  uwrite(uart, DLM,  ((BSPBaseBaud/baud) >> 8) & 0xff);

  /* 8-bit, no parity , 1 stop */
  uwrite(uart, LCR, databits | parity | stopbits);

  /* Set DTR, RTS and OUT2 high */
  uwrite(uart, MCR, DTR | RTS | OUT_2);

  /* Enable FIFO */
  uwrite(uart, FCR, FIFO_EN | XMIT_RESET | RCV_RESET | RECEIVE_FIFO_TRIGGER12);

  /* Disable Interrupts */
  uwrite(uart, IER, 0);

  /* Read status to clear them */
  uread(uart, LSR);
  uread(uart, RBR);
  uread(uart, MSR);

  /* Remember state */
  uart_data[uart].baud       = baud;
  uart_data[uart].databits   = databits;
  uart_data[uart].parity     = parity;
  uart_data[uart].stopbits   = stopbits;
  uart_data[uart].hwFlow     = hwFlow;
  return;
}

/*
 * Set baud
 */

void
BSP_uart_set_baud(
  int uart,
  unsigned long baud
)
{
  /* Sanity check */
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

  BSP_uart_set_attributes( uart, baud, uart_data[uart].databits,
    uart_data[uart].parity, uart_data[uart].stopbits );
}

/*
 *  Set all attributes
 */

void
BSP_uart_set_attributes
(
  int uart,
  unsigned long baud,
  unsigned long databits,
  unsigned long parity,
  unsigned long stopbits
)
{
  unsigned char mcr, ier;

  /* Sanity check */
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

  /*
   * This function may be called whenever TERMIOS parameters
   * are changed, so we have to make sure that baud change is
   * indeed required
   */

  if( (baud     == uart_data[uart].baud)     &&
      (databits == uart_data[uart].databits) &&
      (parity   == uart_data[uart].parity)   &&
      (stopbits == uart_data[uart].stopbits) )
    {
      return;
    }

  mcr = uread(uart, MCR);
  ier = uread(uart, IER);

  BSP_uart_init(uart, baud, databits, parity, stopbits, uart_data[uart].hwFlow);

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
  int iStatus = (int)INTERRUPT_DISABLE;

  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

  switch(cmd)
    {
    case BSP_UART_INTR_CTRL_ENABLE:
      iStatus |= (RECEIVE_ENABLE | RECEIVER_LINE_ST_ENABLE | TRANSMIT_ENABLE);
      if ( uart_data[uart].hwFlow ) {
        iStatus |= MODEM_ENABLE;
      }
      break;
    case BSP_UART_INTR_CTRL_TERMIOS:
      iStatus |= (RECEIVE_ENABLE | RECEIVER_LINE_ST_ENABLE);
      if ( uart_data[uart].hwFlow ) {
        iStatus |= MODEM_ENABLE;
      }
      break;
    case BSP_UART_INTR_CTRL_GDB:
      iStatus |= RECEIVE_ENABLE;
      break;
    }

  uart_data[uart].ier = iStatus;
  uwrite(uart, IER, iStatus);

  return;
}

void
BSP_uart_throttle(int uart)
{
  unsigned int mcr;

  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

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

  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

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

  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

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
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

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

  /*
   * Wait for character to be transmitted.
   * This ensures that printk and printf play nicely together
   * when using the same serial port.
   * Yes, there's a performance hit here, but if we're doing
   * polled writes to a serial port we're probably not that
   * interested in efficiency anyway.....
   */
  for(;;)
    {
      if((val1=uread(uart, LSR)) & THRE)
      {
      break;
      }
    }

  return;
}

void
BSP_output_char_via_serial(char val)
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

  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

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

int
BSP_poll_char_via_serial(void)
{
	return BSP_uart_polled_read(BSPConsolePort);
}

/* ================ Termios support  =================*/

static volatile int  termios_stopped_com1        = 0;
static volatile int  termios_tx_active_com1      = 0;
static void*	     termios_ttyp_com1           = NULL;
static char          termios_tx_hold_com1        = 0;
static volatile char termios_tx_hold_valid_com1  = 0;

static volatile int  termios_stopped_com2        = 0;
static volatile int  termios_tx_active_com2      = 0;
static void*	     termios_ttyp_com2           = NULL;
static char          termios_tx_hold_com2        = 0;
static volatile char termios_tx_hold_valid_com2  = 0;

static void ( *driver_input_handler_com1 )( void *,  char *, int ) = 0;
static void ( *driver_input_handler_com2 )( void *,  char *, int ) = 0;

/*
 * This routine sets the handler to handle the characters received
 * from the serial port.
 */
void uart_set_driver_handler( int port, void ( *handler )( void *,  char *, int ) )
{
  switch( port )
  {
    case BSP_UART_COM1:
     driver_input_handler_com1 = handler;
     break;

    case BSP_UART_COM2:
     driver_input_handler_com2 = handler;
     break;
  }
}

/*
 * Set channel parameters
 */
void
BSP_uart_termios_set(int uart, void *ttyp)
{
  struct rtems_termios_tty *p = (struct rtems_termios_tty *)ttyp;
  unsigned char val;
  assert(uart == BSP_UART_COM1 || uart == BSP_UART_COM2);

  if(uart == BSP_UART_COM1)
    {
      uart_data[uart].ioMode = p->device.outputUsesInterrupts;
      if(uart_data[uart].hwFlow)
	{
	  val = uread(uart, MSR);

	  termios_stopped_com1   = (val & CTS) ? 0 : 1;
	}
      else
	{
	  termios_stopped_com1 = 0;
	}
      termios_tx_active_com1      = 0;
      termios_ttyp_com1           = ttyp;
      termios_tx_hold_com1        = 0;
      termios_tx_hold_valid_com1  = 0;
    }
  else
    {
      uart_data[uart].ioMode = p->device.outputUsesInterrupts;
      if(uart_data[uart].hwFlow)
	{
	  val = uread(uart, MSR);

	  termios_stopped_com2   = (val & CTS) ? 0 : 1;
	}
      else
	{
	  termios_stopped_com2 = 0;
	}
      termios_tx_active_com2      = 0;
      termios_ttyp_com2           = ttyp;
      termios_tx_hold_com2        = 0;
      termios_tx_hold_valid_com2  = 0;
    }

  return;
}

int
BSP_uart_termios_read_com1(int uart)
{
  int     off = (int)0;
  char    buf[40];

  /* read bytes */
  while (( off < sizeof(buf) ) && ( uread(BSP_UART_COM1, LSR) & DR )) {
    buf[off++] = uread(BSP_UART_COM1, RBR);
  }

  /* write out data */
  if ( off > 0 ) {
    rtems_termios_enqueue_raw_characters(termios_ttyp_com1, buf, off);
  }

  /* enable receive interrupts */
  uart_data[BSP_UART_COM1].ier |= (RECEIVE_ENABLE | RECEIVER_LINE_ST_ENABLE);
  uwrite(BSP_UART_COM1, IER, uart_data[BSP_UART_COM1].ier);

  return ( EOF );
}

int
BSP_uart_termios_read_com2(int uart)
{
  int     off = (int)0;
  char    buf[40];

  /* read current byte */
  while (( off < sizeof(buf) ) && ( uread(BSP_UART_COM2, LSR) & DR )) {
    buf[off++] = uread(BSP_UART_COM2, RBR);
  }

  /* write out data */
  if ( off > 0 ) {
    rtems_termios_enqueue_raw_characters(termios_ttyp_com2, buf, off);
  }

  /* enable receive interrupts */
  uart_data[BSP_UART_COM2].ier |= (RECEIVE_ENABLE | RECEIVER_LINE_ST_ENABLE);
  uwrite(BSP_UART_COM2, IER, uart_data[BSP_UART_COM2].ier);

  return ( EOF );
}

ssize_t
BSP_uart_termios_write_com1(int minor, const char *buf, size_t len)
{
  assert(buf != NULL);

  if(len <= 0)
    {
      return 0;
    }

  /* If there TX buffer is busy - something is royally screwed up */
  assert((uread(BSP_UART_COM1, LSR) & THRE) != 0);

  if(termios_stopped_com1)
    {
      /* CTS low */
      termios_tx_hold_com1       = *buf;
      termios_tx_hold_valid_com1 = 1;
      return 0;
    }

  /* Write character */
  uwrite(BSP_UART_COM1, THR, *buf & 0xff);

  /* Enable interrupts if necessary */
  if ( !termios_tx_active_com1 ) {
    termios_tx_active_com1 = 1;
    uart_data[BSP_UART_COM1].ier |= TRANSMIT_ENABLE;
    uwrite(BSP_UART_COM1, IER, uart_data[BSP_UART_COM1].ier);
  }

  return 1;
}

ssize_t
BSP_uart_termios_write_com2(int minor, const char *buf, size_t len)
{
  assert(buf != NULL);

  if(len <= 0)
    {
      return 0;
    }

  /* If there TX buffer is busy - something is royally screwed up */
  assert((uread(BSP_UART_COM2, LSR) & THRE) != 0);

  if(termios_stopped_com2)
    {
      /* CTS low */
      termios_tx_hold_com2       = *buf;
      termios_tx_hold_valid_com2 = 1;
      return 0;
    }

  /* Write character */
  uwrite(BSP_UART_COM2, THR, *buf & 0xff);

  /* Enable interrupts if necessary */
  if ( !termios_tx_active_com2 ) {
    termios_tx_active_com2 = 1;
    uart_data[BSP_UART_COM2].ier |= TRANSMIT_ENABLE;
    uwrite(BSP_UART_COM2, IER, uart_data[BSP_UART_COM2].ier);
  }

  return 1;
}

void
BSP_uart_termios_isr_com1(void)
{
  unsigned char buf[40];
  unsigned char val;
  int      off, ret, vect;

  off = 0;

  for(;;)
    {
      vect = uread(BSP_UART_COM1, IIR) & 0xf;

      switch(vect)
	{
	case MODEM_STATUS :
	  val = uread(BSP_UART_COM1, MSR);
	  if(uart_data[BSP_UART_COM1].hwFlow)
	    {
	      if(val & CTS)
		{
		  /* CTS high */
		  termios_stopped_com1 = 0;
		  if(termios_tx_hold_valid_com1)
		    {
		      termios_tx_hold_valid_com1 = 0;
		      BSP_uart_termios_write_com1(0, &termios_tx_hold_com1,
						    1);
		    }
		}
	      else
		{
		  /* CTS low */
		  termios_stopped_com1 = 1;
		}
	    }
	  break;
	case NO_MORE_INTR :
	  /* No more interrupts */
	  if(off != 0)
	    {
	      /* Update rx buffer */
         if( driver_input_handler_com1 )
         {
             driver_input_handler_com1( termios_ttyp_com1, (char *)buf, off );
         }
         else
         {
            /* Update rx buffer */
	         rtems_termios_enqueue_raw_characters(termios_ttyp_com1, (char *)buf, off );
         }
	    }
	  return;
	case TRANSMITTER_HODING_REGISTER_EMPTY :
	  /*
	   * TX holding empty: we have to disable these interrupts
	   * if there is nothing more to send.
	   */

	  /* If nothing else to send disable interrupts */
	  ret = rtems_termios_dequeue_characters(termios_ttyp_com1, 1);
          if ( ret == 0 ) {
            termios_tx_active_com1 = 0;
            uart_data[BSP_UART_COM1].ier &= ~(TRANSMIT_ENABLE);
            uwrite(BSP_UART_COM1, IER, uart_data[BSP_UART_COM1].ier);
          }
	  break;
	case RECEIVER_DATA_AVAIL :
	case CHARACTER_TIMEOUT_INDICATION:
          if ( uart_data[BSP_UART_COM1].ioMode == TERMIOS_TASK_DRIVEN ) {
            /* ensure interrupts are enabled */
            if ( uart_data[BSP_UART_COM1].ier & RECEIVE_ENABLE ) {
              /* disable interrupts and notify termios */
              uart_data[BSP_UART_COM1].ier &= ~(RECEIVE_ENABLE | RECEIVER_LINE_ST_ENABLE);
              uwrite(BSP_UART_COM1, IER, uart_data[BSP_UART_COM1].ier);
              rtems_termios_rxirq_occured(termios_ttyp_com1);
            }
          }
          else {
	    /* RX data ready */
	    assert(off < sizeof(buf));
	    buf[off++] = uread(BSP_UART_COM1, RBR);
          }
	  break;
	case RECEIVER_ERROR:
	  /* RX error: eat character */
	   uartError(BSP_UART_COM1);
	  break;
	default:
	  /* Should not happen */
	  assert(0);
	  return;
	}
    }
}

void
BSP_uart_termios_isr_com2(void)
{
  unsigned char buf[40];
  unsigned char val;
  int      off, ret, vect;

  off = 0;

  for(;;)
    {
      vect = uread(BSP_UART_COM2, IIR) & 0xf;

      switch(vect)
	{
	case MODEM_STATUS :
	  val = uread(BSP_UART_COM2, MSR);
	  if(uart_data[BSP_UART_COM2].hwFlow)
	    {
	      if(val & CTS)
		{
		  /* CTS high */
		  termios_stopped_com2 = 0;
		  if(termios_tx_hold_valid_com2)
		    {
		      termios_tx_hold_valid_com2 = 0;
		      BSP_uart_termios_write_com2(0, &termios_tx_hold_com2,
						    1);
		    }
		}
	      else
		{
		  /* CTS low */
		  termios_stopped_com2 = 1;
		}
	    }
	  break;
	case NO_MORE_INTR :
	  /* No more interrupts */
	  if(off != 0)
	    {
	      /* Update rx buffer */
         if( driver_input_handler_com2 )
         {
             driver_input_handler_com2( termios_ttyp_com2, (char *)buf, off );
         }
         else
         {
	        rtems_termios_enqueue_raw_characters(termios_ttyp_com2, (char *)buf, off);
         }
	    }
	  return;
	case TRANSMITTER_HODING_REGISTER_EMPTY :
	  /*
	   * TX holding empty: we have to disable these interrupts
	   * if there is nothing more to send.
	   */

	  /* If nothing else to send disable interrupts */
	  ret = rtems_termios_dequeue_characters(termios_ttyp_com2, 1);
          if ( ret == 0 ) {
            termios_tx_active_com2 = 0;
            uart_data[BSP_UART_COM2].ier &= ~(TRANSMIT_ENABLE);
            uwrite(BSP_UART_COM2, IER, uart_data[BSP_UART_COM2].ier);
          }
	  break;
	case RECEIVER_DATA_AVAIL :
	case CHARACTER_TIMEOUT_INDICATION:
          if ( uart_data[BSP_UART_COM2].ioMode == TERMIOS_TASK_DRIVEN ) {
            /* ensure interrupts are enabled */
            if ( uart_data[BSP_UART_COM2].ier & RECEIVE_ENABLE ) {
              /* disable interrupts and notify termios */
              uart_data[BSP_UART_COM2].ier &= ~(RECEIVE_ENABLE | RECEIVER_LINE_ST_ENABLE);
              uwrite(BSP_UART_COM2, IER, uart_data[BSP_UART_COM2].ier);
              rtems_termios_rxirq_occured(termios_ttyp_com2);
            }
          }
          else {
	    /* RX data ready */
	    assert(off < sizeof(buf));
	    buf[off++] = uread(BSP_UART_COM2, RBR);
          }
	  break;
	case RECEIVER_ERROR:
	  /* RX error: eat character */
	   uartError(BSP_UART_COM2);
	  break;
	default:
	  /* Should not happen */
	  assert(0);
	  return;
	}
    }
}

/* ================= GDB support     ===================*/
int BSP_uart_dbgisr_com_regsav[4] __attribute__ ((unused));

/*
 * Interrupt service routine for COM1 - all,
 * it does it check whether ^C is received
 * if yes it will flip TF bit before returning
 * Note: it should be installed as raw interrupt
 * handler
 */

__asm__ (".p2align 4");
__asm__ (".text");
__asm__ (".globl BSP_uart_dbgisr_com1");
__asm__ ("BSP_uart_dbgisr_com1:");
__asm__ ("    movl %eax, BSP_uart_dbgisr_com_regsav");          /* Save eax */
__asm__ ("    movl %ebx, BSP_uart_dbgisr_com_regsav + 4");      /* Save ebx */
__asm__ ("    movl %edx, BSP_uart_dbgisr_com_regsav + 8");      /* Save edx */

__asm__ ("    movl $0, %ebx");           /* Clear flag */

/*
 * We know that only receive related interrupts
 * are available, eat chars
 */
__asm__ ("uart_dbgisr_com1_1:");
__asm__ ("    movw $0x3FD, %dx");
__asm__ ("    inb  %dx, %al"); /* Read LSR */
__asm__ ("    andb $1, %al");
__asm__ ("    cmpb $0, %al");
__asm__ ("    je   uart_dbgisr_com1_2");
__asm__ ("    movw $0x3F8, %dx");
__asm__ ("    inb  %dx, %al");    /* Get input character */
__asm__ ("    cmpb $3, %al");
__asm__ ("    jne  uart_dbgisr_com1_1");

/* ^C received, set flag */
__asm__ ("    movl $1, %ebx");
__asm__ ("    jmp uart_dbgisr_com1_1");

/* All chars read */
__asm__ ("uart_dbgisr_com1_2:");

/* If flag is set we have to tweak TF */
__asm__ ("   cmpl $0, %ebx");
__asm__ ("   je   uart_dbgisr_com1_3");

/* Flag is set */
__asm__ ("   movl BSP_uart_dbgisr_com_regsav+4, %ebx");     /* Restore ebx */
__asm__ ("   movl BSP_uart_dbgisr_com_regsav+8, %edx");     /* Restore edx */

/* Set TF bit */
__asm__ ("   popl  %eax");           			 /* Pop eip */
__asm__ ("   movl  %eax, BSP_uart_dbgisr_com_regsav + 4");  /* Save it */
__asm__ ("   popl  %eax");           			 /* Pop cs */
__asm__ ("   movl  %eax, BSP_uart_dbgisr_com_regsav + 8");  /* Save it */
__asm__ ("   popl  %eax");           			 /* Pop flags */
__asm__ ("   orl   $0x100, %eax");   			 /* Modify it */
__asm__ ("   pushl %eax");           			 /* Push it back */
__asm__ ("   movl  BSP_uart_dbgisr_com_regsav+8, %eax");    /* Put back cs */
__asm__ ("   pushl %eax");
__asm__ ("   movl  BSP_uart_dbgisr_com_regsav+4, %eax");    /* Put back eip */
__asm__ ("   pushl %eax");

/* Acknowledge IRQ */
__asm__ ("   movb  $0x20, %al");
__asm__ ("   outb  %al, $0x20");
__asm__ ("   movl  BSP_uart_dbgisr_com_regsav, %eax");      /* Restore eax */
__asm__ ("   iret");                 			 /* Done */

/* Flag is not set */
__asm__ ("uart_dbgisr_com1_3:");
__asm__ ("   movl BSP_uart_dbgisr_com_regsav+4, %ebx");     /* Restore ebx */
__asm__ ("   movl BSP_uart_dbgisr_com_regsav+8, %edx");     /* Restore edx */

/* Acknowledge irq */
__asm__ ("   movb  $0x20, %al");
__asm__ ("   outb  %al, $0x20");
__asm__ ("   movl  BSP_uart_dbgisr_com_regsav, %eax");      /* Restore eax */
__asm__ ("   iret");                 /* Done */

/*
 * Interrupt service routine for COM2 - all,
 * it does it check whether ^C is received
 * if yes it will flip TF bit before returning
 * Note: it has to be installed as raw interrupt
 * handler
 */
__asm__ (".p2align 4");
__asm__ (".text");
__asm__ (".globl BSP_uart_dbgisr_com2");
__asm__ ("BSP_uart_dbgisr_com2:");
__asm__ ("    movl %eax, BSP_uart_dbgisr_com_regsav");          /* Save eax */
__asm__ ("    movl %ebx, BSP_uart_dbgisr_com_regsav + 4");      /* Save ebx */
__asm__ ("    movl %edx, BSP_uart_dbgisr_com_regsav + 8");      /* Save edx */

__asm__ ("    movl $0, %ebx");           /* Clear flag */

/*
 * We know that only receive related interrupts
 * are available, eat chars
 */
__asm__ ("uart_dbgisr_com2_1:");
__asm__ ("    movw $0x2FD, %dx");
__asm__ ("    inb  %dx, %al"); /* Read LSR */
__asm__ ("    andb $1, %al");
__asm__ ("    cmpb $0, %al");
__asm__ ("    je   uart_dbgisr_com2_2");
__asm__ ("    movw $0x2F8, %dx");
__asm__ ("    inb  %dx, %al");    /* Get input character */
__asm__ ("    cmpb $3, %al");
__asm__ ("    jne  uart_dbgisr_com2_1");

/* ^C received, set flag */
__asm__ ("    movl $1, %ebx");
__asm__ ("    jmp uart_dbgisr_com2_1");

/* All chars read */
__asm__ ("uart_dbgisr_com2_2:");

/* If flag is set we have to tweak TF */
__asm__ ("   cmpl $0, %ebx");
__asm__ ("   je   uart_dbgisr_com2_3");

/* Flag is set */
__asm__ ("   movl BSP_uart_dbgisr_com_regsav+4, %ebx");     /* Restore ebx */
__asm__ ("   movl BSP_uart_dbgisr_com_regsav+8, %edx");     /* Restore edx */

/* Set TF bit */
__asm__ ("   popl  %eax");           /* Pop eip */
__asm__ ("   movl  %eax, BSP_uart_dbgisr_com_regsav + 4");  /* Save it */
__asm__ ("   popl  %eax");           /* Pop cs */
__asm__ ("   movl  %eax, BSP_uart_dbgisr_com_regsav + 8");  /* Save it */
__asm__ ("   popl  %eax");           /* Pop flags */
__asm__ ("   orl   $0x100, %eax");   /* Modify it */
__asm__ ("   pushl %eax");           /* Push it back */
__asm__ ("   movl  BSP_uart_dbgisr_com_regsav+8, %eax");    /* Put back cs */
__asm__ ("   pushl %eax");
__asm__ ("   movl  BSP_uart_dbgisr_com_regsav+4, %eax");    /* Put back eip */
__asm__ ("   pushl %eax");

/* Acknowledge IRQ */
__asm__ ("   movb  $0x20, %al");
__asm__ ("   outb  %al, $0x20");
__asm__ ("   movl  BSP_uart_dbgisr_com_regsav, %eax");      /* Restore eax */
__asm__ ("   iret");                 /* Done */

/* Flag is not set */
__asm__ ("uart_dbgisr_com2_3:");
__asm__ ("   movl BSP_uart_dbgisr_com_regsav+4, %ebx");     /* Restore ebx */
__asm__ ("   movl BSP_uart_dbgisr_com_regsav+8, %edx");     /* Restore edx */

/* Acknowledge irq */
__asm__ ("   movb  $0x20, %al");
__asm__ ("   outb  %al, $0x20");
__asm__ ("   movl  BSP_uart_dbgisr_com_regsav, %eax");      /* Restore eax */
__asm__ ("   iret");                 /* Done */
