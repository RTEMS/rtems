/*
 * Initialize SMC1 for console IO.
 *
 * Based on the `gen68302' board support package, and covered by the
 * original distribution terms.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id$
 */

/*
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 */

#define GEN68360_INIT

#include <bsp.h>
#include <rtems/libio.h>
#include "m68360.h"

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

/*
 * I/O buffers can be in ordindary RAM
 */
static volatile char rxBuf, txBuf;
static volatile m360BufferDescriptor_t *consoleRxBd, *consoleTxBd;

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
	rtems_status_code status;

	/*
	 * Allocate buffer descriptors
	 */
	consoleRxBd = M360AllocateBufferDescriptors (1);
	consoleTxBd = M360AllocateBufferDescriptors (1);

	/*
	 * Configure port B pins to enable SMTXD1 and SMRXD1 pins
	 */
	m360.pbpar |=  0xC0;
	m360.pbdir &= ~0xC0;
	m360.pbodr &= ~0xC0;

	/*
	 * Set up BRG1 (9,600 baud)
	 */
	m360.brgc1 = M360_BRG_RST;
	m360.brgc1 = M360_BRG_EN | M360_BRG_EXTC_BRGCLK | M360_BRG_9600;

	/*
	 * Put SMC1 in NMSI mode, connect SMC1 to BRG1
	 */
	m360.simode |= M360_SI_SMC1_BRG1;
	 
	/*
	 * Set up SMC1 parameter RAM common to all protocols
	 */
	m360.smc1p.rbase = (char *)consoleRxBd - (char *)&m360;
	m360.smc1p.tbase = (char *)consoleTxBd - (char *)&m360;
	m360.smc1p.rfcr = M360_RFCR_MOT | M360_RFCR_DMA_SPACE;
	m360.smc1p.tfcr = M360_TFCR_MOT | M360_TFCR_DMA_SPACE;
	m360.smc1p.mrblr = 1;
	 
	/*
	 * Set up SMC1 parameter RAM UART-specific parameters
	 */
	m360.smc1p.un.uart.max_idl = 0;
	m360.smc1p.un.uart.brklen = 0;
	m360.smc1p.un.uart.brkec = 0;
	m360.smc1p.un.uart.brkcr = 0;
	 
	/*
	 * Set up the Receive Buffer Descriptor
	 */
	consoleRxBd->status = M360_BD_EMPTY | M360_BD_WRAP;
	consoleRxBd->length = 0;
	consoleRxBd->buffer = &rxBuf;
	 
	/*
	 * Setup the Transmit Buffer Descriptor
	 */
	consoleTxBd->length = 1;
	consoleTxBd->status = M360_BD_WRAP;
	consoleTxBd->buffer = &txBuf;
	 
	/*
	 * Set up SMC1 general and protocol-specific mode registers
	 */
	m360.smc1.smce = ~0;	/* Clear any pending events */
	m360.smc1.smcm = 0;	/* Mask all interrupt/event sources */
	m360.smc1.smcmr = M360_SMCMR_CLEN(9) | M360_SMCMR_SM_UART;

	/*
	 * Send "Init parameters" command
	 */
	M360ExecuteRISC (M360_CR_OP_INIT_RX_TX | M360_CR_CHAN_SMC1);

	/*
	 * Enable receiver and transmitter
	 */
	m360.smc1.smcmr |= M360_SMCMR_TEN | M360_SMCMR_REN;

	status = rtems_io_register_name(
				"/dev/console",
				    major,
				    (rtems_device_minor_number)0);
	if (status != RTEMS_SUCCESSFUL)
		rtems_fatal_error_occurred(status);
	return RTEMS_SUCCESSFUL;
}

/*  is_character_ready
 *
 *  Check to see if a character is available on the console port.  If so,
 *  then return a TRUE (along with the character).  Otherwise return FALSE.
 *
 *  Input parameters:   pointer to location in which to return character
 *
 *  Output parameters:  character (if available)
 *
 *  Return values:      TRUE - character available
 *                      FALSE - no character available
 */

rtems_boolean is_character_ready(
  char *ch				/* -> character  */
)
{
	if (consoleRxBd->status & M360_BD_EMPTY)
		return FALSE;
	*ch = rxBuf;
	consoleRxBd->status = M360_BD_EMPTY | M360_BD_WRAP;
	return TRUE;
}


/*  inbyte
 *
 *  Receive a character from the console port
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:      character read
 */

char inbyte( void )
{
    char ch;

    while (is_character_ready (&ch) == FALSE)
	continue;
    return ch;
}


/*  outbyte
 *
 *  Transmit a character to the console serial port
 *
 *  Input parameters:
 *    ch  - character to be transmitted
 *
 *  Output parameters:  NONE
 */

void outbyte(
  char ch
)
{
	if (ch == '\n')
		outbyte('\r');
	while (consoleTxBd->status & M360_BD_READY)
		continue;
	txBuf = ch;
	consoleTxBd->status = M360_BD_READY | M360_BD_WRAP;
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

  for (count = 0; count < maximum; count++) {
    buffer[ count ] = inbyte();
    if (buffer[ count ] == '\n' || buffer[ count ] == '\r') {
      buffer[ count++ ]  = '\n';
      buffer[ count ]  = 0;
      break;
    }
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
  return RTEMS_SUCCESSFUL;
}
