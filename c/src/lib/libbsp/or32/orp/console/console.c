/*
 *  This file contains the template for a console IO package.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  This file adapted from no_bsp board library of the RTEMS distribution.
 *  The body has been modified for the Bender Or1k implementation by
 *  Chris Ziomkowski. <chris@asics.ws>
 */

#define BENDER_INIT

#include <bsp.h>
#include <rtems/libio.h>
#include "console.h"

static int localEcho;
static UART_16450* uart = (UART_16450*)0x80000000;  /* This is where the simulator puts it */

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

void (*old_handler)(unsigned int,unsigned int,unsigned int,unsigned int);

void console_interrupt(unsigned int vector,unsigned int pc,
		       unsigned int effective_addr, unsigned int status)
{
  int reason;
  register int pending;

  /* First thing's first...is this for us? */
  asm volatile ("l.mfspr %0,r0,0x4802 \n\t"  /* Read the PIC status */
		"l.andi  %0,%0,0x4    \n\t" : "=r" (pending));

  if(pending)
    {
      reason = uart->read.IIR;
      
      switch(reason)
	{
	case 0: /* Interrupt because of modem status */
	  break;
	case 2: /* Interrupt because Transmitter empty */
	  break;
	case 4: /* Interrupt because Received data available */
	  break;
	case 6: /* Interrupt because Status Register */
	  break;
	case 12: /* Interrupt because of character timeout (16550 only) */
	  break;
	default: /* No interrupt */
	  break;
	}
    }

  if(old_handler)
    (*old_handler)(vector,pc,effective_addr,status);
}

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;
  int tmp,tmp2;
  unsigned32 sr;
  extern unsigned32 Or1k_Interrupt_Vectors[16];

  /* Make sure the UART (interrupt 2) is enabled and
     reports a low prority interrupt */
  asm volatile ("l.mfspr %0,r0,0x4800  \n\t"  /* Get the PIC mask */
		"l.ori   %0,%0,0x4     \n\t"  /* Enable int 2 */
		"l.mtspr r0,%0,0x4800  \n\t"  /* Write back mask */
		"l.mfspr %0,r0,0x4801  \n\t"  /* Get priority mask */
		"l.addi  %1,r0,-5      \n\t"
		"l.and   %0,%0,%1      \n\t"  /* Set us to low */
		"l.mtspr r0,%0,0x4801  \n\t"  /* Write back to PICPR */
		: "=r" (tmp), "=r" (tmp2));

  /* Install the interrupt handler */
  asm volatile ("l.mfspr %0,r0,0x11 \n\t"
		"l.addi  %1,r0,-5   \n\t"
		"l.and   %1,%1,%0   \n\t"
		"l.mtspr r0,%1,0x11 \n\t": "=&r" (sr) : "r" (tmp));

  old_handler = (void(*)(unsigned int,unsigned int,unsigned int,unsigned int))
    Or1k_Interrupt_Vectors[5];
  Or1k_Interrupt_Vectors[5] = (unsigned32)console_interrupt;

  asm volatile ("l.mtspr r0,%0,0x11\n\t":: "r" (sr));

  /* Assume 1843.2/16 kHz clock */
  uart->latch.LCR = 0x80;    /* Set the divisor latch bit */
  uart->latch.DLM = 2;       /* 57,600 */
  uart->latch.DLL = 0;
  uart->write.LCR = 0x03;    /* 8-N-1 */
  uart->write.MCR = 0x03;    /* Assert RTS & DTR */
  /* uart->write.FCR = 0x00; */ /* Make sure we're in 16450 mode... Ignore for 16450 driver */
  uart->write.IER = 0x05;    /* Don't worry about TEMT unless we need to. */

  tmp = uart->read.LSR;     /* Make sure interrupts are cleared */
  tmp = uart->read.RBR;     /* Clear the input buffer */
  tmp = uart->read.MSR;     /* Clear the modem status register */

  localEcho = 1;       /* Turn on local echo */

  status = rtems_io_register_name(
    "/dev/console",
    major,
    (rtems_device_minor_number) 0
  );
 
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);
 
  return RTEMS_SUCCESSFUL;
}


/*  is_character_ready
 *
 *  This routine returns TRUE if a character is available.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

rtems_boolean is_character_ready(
  char *ch
)
{
  *ch = '\0';   /* return NULL for no particular reason */
  return(TRUE);
}

/*  inbyte
 *
 *  This routine reads a character from the SOURCE.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    character read from SOURCE
 */

char inbyte( void )
{
  unsigned int stat;

  stat = uart->read.LSR;
  while(!(stat & 0x01))  /* ! Data Ready */
    {
      rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
      stat = uart->read.LSR;
    }

  return uart->read.RBR;   /* Return the character */
}

/*  outbyte
 *
 *  This routine transmits a character out the SOURCE.  Flow
 *  control is not currently enabled.
 *
 *  Input parameters:
 *    ch  - character to be transmitted
 *
 *  Output parameters:  NONE
 */

void outbyte(char ch)
{
  unsigned int stat;
  /*
   *  Carriage Return/New line translation.
   */

  stat = uart->read.LSR;
  while(!(stat & 0x40))  /* ! TEMT */
    {
      rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
      stat = uart->read.LSR;
    }

  uart->write.THR = ch;
  if ( ch == '\n' )
    outbyte( '\r' );
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
  return RTEMS_SUCCESSFUL;
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
  return RTEMS_SUCCESSFUL;
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
  rtems_libio_rw_args_t *rw_args;
  char *buffer;
  int maximum;
  int count = 0;
 
  rw_args = (rtems_libio_rw_args_t *) arg;

  buffer = rw_args->buffer;
  maximum = rw_args->count;

  
  for (count = 0; count < maximum; count++)
    {
      buffer[ count ] = inbyte();

      if (buffer[ count ] == '\n' || buffer[ count ] == '\r')
	{
	  buffer[ count++ ]  = '\n';
	  if(localEcho)
	    outbyte('\n' ); /* newline */
	  break;  /* Return for a newline */
	}
      else if (buffer[ count ] == '\b' && count > 0 )
	{
	  if(localEcho)
	    {
	      outbyte('\b' ); /* move back one space */
	      outbyte(' ' ); /* erase the character */
	      outbyte('\b' ); /* move back one space */
	    }
	  count-=2;
	}
      else if(localEcho)
	outbyte(buffer[ count ]); /* echo the character */
    }

  rw_args->bytes_moved = count;
  return (count >= 0) ? RTEMS_SUCCESSFUL : RTEMS_UNSATISFIED;
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
  int count;
  int maximum;
  rtems_libio_rw_args_t *rw_args;
  char *buffer;

  rw_args = (rtems_libio_rw_args_t *) arg;

  buffer = rw_args->buffer;
  maximum = rw_args->count;

  for (count = 0; count < maximum; count++) {
    if ( buffer[ count ] == '\n') {
      outbyte('\r');
    }
    outbyte( buffer[ count ] );
  }

  rw_args->bytes_moved = maximum;
  return 0;
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
  int param,div;
  ConsoleIOCTLRequest* request = (ConsoleIOCTLRequest*)arg;

  if (!arg)
    return RTEMS_INVALID_ADDRESS;

  switch(request->command)
    {
    case TERM_LOCAL_ECHO:
      param = (int)(request->data);
      if(param < 0 || param > 1)
	return RTEMS_INVALID_NUMBER;
      localEcho = param;
      break;
    case TERM_BIT_RATE:
      param = (int)(request->data);
      switch(param)
	{
	case 50: 
        case 150:
	case 300:
	case 600:
	case 1200:
	case 1800:
	case 2000:
	case 2400:
	case 3600:
	case 4800:
	case 7200:
	case 9600:
	case 19200:
	case 38400:
	case 57600:
	case 115200:
	  div = 115200/param;
	  uart->latch.LCR |= 0x80;    /* Set the divisor latch bit */
	  uart->latch.DLM = div & 0xFF;
	  uart->latch.DLL = div >> 8;
	  uart->write.LCR &= 0x7F;    /* Clear the divisor latch bit */
	  break;
	default:
	  return RTEMS_INVALID_NUMBER;
	}
      break;
    default:
      return RTEMS_NOT_CONFIGURED;
    }

  return RTEMS_SUCCESSFUL;
}
