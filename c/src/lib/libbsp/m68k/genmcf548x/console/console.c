/*===============================================================*\
| Project: RTEMS generic mcf548x BSP                              |
+-----------------------------------------------------------------+
| File: console.c                                                 |
+-----------------------------------------------------------------+
| The file contains the console driver code of generic MCF548x    |
| BSP.                                                            |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
|                                                                 |
| Parts of the code has been derived from the "dBUG source code"  |
| package Freescale is providing for M548X EVBs. The usage of     |
| the modified or unmodified code and it's integration into the   |
| generic mcf548x BSP has been done according to the Freescale    |
| license terms.                                                  |
|                                                                 |
| The Freescale license terms can be reviewed in the file         |
|                                                                 |
|    Freescale_license.txt                                        |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
| The generic mcf548x BSP has been developed on the basic         |
| structures and modules of the av5282 BSP.                       |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
|                                                                 |
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 12.11.07                    1.0                            ras  |
|                                                                 |
\*===============================================================*/

 /*
  *  Multi UART console serial I/O.
  *
  * TO DO: Add DMA input/output
  */

#include <stdio.h>
#include <fcntl.h>
#include <rtems/libio.h>
#include <rtems/termiostypes.h>
#include <termios.h>
#include <bsp.h>
#include <malloc.h>
#include <rtems/mw_uid.h>

#include <rtems/bspIo.h>

#define UART_INTC0_IRQ_VECTOR(x) (64+35-(x))

#define MCF548X_PSC_SR_ERROR ( MCF548X_PSC_SR_RB_NEOF   | \
                           MCF548X_PSC_SR_FE_PHYERR | \
                           MCF548X_PSC_SR_PE_CRCERR | \
                           MCF548X_PSC_SR_OE )

static ssize_t IntUartPollWrite(int minor, const char *buf, size_t len);
static int IntUartPollRead (int minor);

static void
_BSP_null_char( char c )
{
	int level;

    if (c == '\n')
        _BSP_null_char('\r');
	rtems_interrupt_disable(level);
    while (!((MCF548X_PSC_SR(CONSOLE_PORT) & MCF548X_PSC_SR_TXRDY)))
        continue;
    *((uint8_t *) &MCF548X_PSC_TB(CONSOLE_PORT)) = c;
    while (!((MCF548X_PSC_SR(CONSOLE_PORT) & MCF548X_PSC_SR_TXRDY)))
        continue;
	rtems_interrupt_enable(level);
}
BSP_output_char_function_type     BSP_output_char = _BSP_null_char;
BSP_polling_getchar_function_type BSP_poll_char = NULL;

#define MAX_UART_INFO     4
#define RX_BUFFER_SIZE    248

struct IntUartInfoStruct
{
	int                    iomode;
	volatile int           imr;
	int                    baud;
	int                    databits;
	int                    parity;
	int                    stopbits;
	int                    hwflow;
	int                    rx_in;
	int                    rx_out;
	char                   rx_buffer[RX_BUFFER_SIZE];
	void                  *ttyp;
};

struct IntUartInfoStruct IntUartInfo[MAX_UART_INFO];

static int GetBaud( int baudHandle )
{
	int baud = BSP_CONSOLE_BAUD;
	switch(baudHandle)
	{
		case B0:
			baud = (int)0;
			break;
		case B1200:
			baud = (int)1200;
			break;
		case B2400:
			baud = (int)2400;
			break;
		case B4800:
			baud = (int)4800;
			break;
		case B9600:
			baud = (int)9600;
			break;
		case B19200:
			baud = (int)19200;
			break;
		case B38400:
			baud = (int)38400;
			break;
		case B57600:
			baud = (int)57600;
			break;
		case B115200:
			baud = (int)115200;
			break;
	}
	return baud;
}

/***************************************************************************
   Function : IntUartSet

   Description : This updates the hardware UART settings.
 ***************************************************************************/
static void
IntUartSet(int minor, int baud, int databits, int parity, int stopbits, int hwflow)
{
	uint8_t                    psc_mode_1 = 0, psc_mode_2 = 0;
	uint16_t                   divider;
	int                        level;
	struct IntUartInfoStruct   *info = &IntUartInfo[minor];

	rtems_interrupt_disable(level);

	/* disable interrupts, clear RTS line, and disable the UARTS */
	/* Mask all psc interrupts */
	MCF548X_PSC_IMR(minor) = 0x0000;

	/* Clear RTS to send */
	MCF548X_PSC_OPSET(minor) &= ~(MCF548X_PSC_OPSET_RTS);

	/* Disable receiver and transmitter */
    MCF548X_PSC_CR(minor) &= ~(MCF548X_PSC_CR_RX_ENABLED | MCF548X_PSC_CR_TX_ENABLED);

    /* provide gpio settings */
    switch (minor)
	  {
      case 0:
        MCF548X_GPIO_PAR_PSC0    = (0 | MCF548X_GPIO_PAR_PSC0_PAR_TXD0 | MCF548X_GPIO_PAR_PSC0_PAR_RXD0);

        if(hwflow)
          {
          MCF548X_GPIO_PAR_PSC0 |= (0 | MCF548X_GPIO_PAR_PSC0_PAR_CTS0_CTS | MCF548X_GPIO_PAR_PSC0_PAR_RTS0_RTS);
          }
        break;
      case 1:
        MCF548X_GPIO_PAR_PSC1    = (0 | MCF548X_GPIO_PAR_PSC1_PAR_TXD1 | MCF548X_GPIO_PAR_PSC1_PAR_RXD1);

        if(hwflow)
          {
          MCF548X_GPIO_PAR_PSC1 |= (0 | MCF548X_GPIO_PAR_PSC1_PAR_CTS1_CTS | MCF548X_GPIO_PAR_PSC1_PAR_RTS1_RTS);
          }
        break;
      case 2:
        MCF548X_GPIO_PAR_PSC2    = (0 | MCF548X_GPIO_PAR_PSC2_PAR_TXD2 | MCF548X_GPIO_PAR_PSC2_PAR_RXD2);

        if(hwflow)
          {
          MCF548X_GPIO_PAR_PSC2 |= (0 | MCF548X_GPIO_PAR_PSC2_PAR_CTS2_CTS | MCF548X_GPIO_PAR_PSC2_PAR_RTS2_RTS);
          }
        break;
      case 3:
        MCF548X_GPIO_PAR_PSC3    = (0 | MCF548X_GPIO_PAR_PSC3_PAR_TXD3 | MCF548X_GPIO_PAR_PSC3_PAR_RXD3);

        if(hwflow)
          {
          MCF548X_GPIO_PAR_PSC3 |= (0 | MCF548X_GPIO_PAR_PSC3_PAR_CTS3_CTS | MCF548X_GPIO_PAR_PSC3_PAR_RTS3_RTS);
          }
        break;
      default:
        break;
	}

    /* save the current values */
	info->imr      = 0;
	info->baud     = baud;
	info->databits = databits;
	info->parity   = parity;
	info->stopbits = stopbits;
	info->hwflow   = hwflow;

    /* Put PSC in UART mode */
	MCF548X_PSC_SICR(minor) = MCF548X_PSC_SICR_SIM_UART;

    /* set the baud rate values */
	MCF548X_PSC_CSR(minor) = (0 | MCF548X_PSC_CSR_RCSEL_SYS_CLK | MCF548X_PSC_CSR_TCSEL_SYS_CLK);

	/* Calculate baud settings */
	divider = (uint16_t)((get_CPU_clock_speed())/(baud * 32));
	MCF548X_PSC_CTUR(minor) =  (uint8_t) ((divider >> 8) & 0xFF);
	MCF548X_PSC_CTLR(minor) =  (uint8_t) (divider & 0xFF);

	/* Reset transmitter, receiver, mode register, and error conditions */
	MCF548X_PSC_CR(minor) = MCF548X_PSC_CR_RESET_RX;
	MCF548X_PSC_CR(minor) = MCF548X_PSC_CR_RESET_TX;
	MCF548X_PSC_CR(minor) = MCF548X_PSC_CR_RESET_ERROR;
	MCF548X_PSC_CR(minor) = MCF548X_PSC_CR_BKCHGINT;
	MCF548X_PSC_CR(minor) = MCF548X_PSC_CR_RESET_MR;

	/* check to see if doing hardware flow control */
	if ( hwflow )
	{
		/* set hardware flow options */
		psc_mode_1 = MCF548X_PSC_MR_RXRTS;
        psc_mode_2 = MCF548X_PSC_MR_TXCTS;
	}

    /* set mode registers */
	psc_mode_1 |= (uint8_t)(parity | databits);
    psc_mode_2 |= (uint8_t)(stopbits);

    /* set mode registers */
	MCF548X_PSC_MR(minor) = psc_mode_1;
    MCF548X_PSC_MR(minor) = psc_mode_2;

    /* Setup FIFO Alarms */
    MCF548X_PSC_RFAR(minor) = MCF548X_PSC_RFAR_ALARM(248);
	MCF548X_PSC_TFAR(minor) = MCF548X_PSC_TFAR_ALARM(248);

	/* check to see if interrupts need to be enabled */
	if ( info->iomode != TERMIOS_POLLED )
	{
		/* enable rx interrupts */
		info->imr |= MCF548X_PSC_IMR_RXRDY_FU;
		MCF548X_PSC_IMR(minor) = info->imr;
	}

	/* check to see if doing hardware flow control */
	if ( hwflow )
	{
		/* assert the RTS line */
		MCF548X_PSC_OPSET(minor) = MCF548X_PSC_OPSET_RTS;
	}

	rtems_interrupt_enable(level);

    /* Enable receiver and transmitter */
	MCF548X_PSC_CR(minor) =(0 | MCF548X_PSC_CR_RX_ENABLED | MCF548X_PSC_CR_TX_ENABLED);


}

/***************************************************************************
   Function : IntUartSetAttributes

   Description : This provides the hardware-dependent portion of tcsetattr().
   value and sets it. At the moment this just sets the baud rate.

   Note: The highest baudrate is 115200 as this stays within
   an error of +/- 5% at 25MHz processor clock
 ***************************************************************************/
static int
IntUartSetAttributes(int minor, const struct termios *t)
{
/* set default index values */
#ifdef HAS_DBUG
	int                         baud     = DBUG_SETTINGS.console_baudrate;
#else
	int                         baud     = (int)BSP_CONSOLE_BAUD;
#endif
	int                         databits = (int)MCF548X_PSC_MR_BC_8;
	int                         parity   = (int)MCF548X_PSC_MR_PM_NONE;
	int                         stopbits = (int)MCF548X_PSC_MR_SB_STOP_BITS_1;
	int                         hwflow   = (int)1;
	struct IntUartInfoStruct   *info     = &IntUartInfo[minor];

	/* check to see if input is valid */
	if ( t != (const struct termios *)0 )
	{
		/* determine baud rate index */
		baud = GetBaud( t->c_cflag & CBAUD );

		/* determine data bits */
		switch ( t->c_cflag & CSIZE )
		{
			case CS5:
				databits = (int)MCF548X_PSC_MR_BC_5;
				break;
			case CS6:
				databits = (int)MCF548X_PSC_MR_BC_6;
				break;
			case CS7:
				databits = (int)MCF548X_PSC_MR_BC_7;
				break;
			case CS8:
				databits = (int)MCF548X_PSC_MR_BC_8;
				break;
		}

		/* determine if parity is enabled */
		if ( t->c_cflag & PARENB )
		{
			if ( t->c_cflag & PARODD )
			{
				/* odd parity */
				parity = (int)MCF548X_PSC_MR_PM_ODD;
			}
			else
			{
				/* even parity */
				parity = (int)MCF548X_PSC_MR_PM_EVEN;
			}
		}

		/* determine stop bits */
		if ( t->c_cflag & CSTOPB )
		{
			/* two stop bits */
			stopbits = (int)MCF548X_PSC_MR_SB_STOP_BITS_2;
		}

		/* check to see if hardware flow control */
		if ( t->c_cflag & CRTSCTS )
		{
			hwflow = 1;
		}
	}

	/* check to see if values have changed */
	if ( ( baud     != info->baud     ) ||
		 ( databits != info->databits ) ||
		 ( parity   != info->parity   ) ||
		 ( stopbits != info->stopbits ) ||
		 ( hwflow   != info->hwflow   ) )
	{

		/* call function to set values */
		IntUartSet(minor, baud, databits, parity, stopbits, hwflow);
	}

return RTEMS_SUCCESSFUL;

}

/***************************************************************************
   Function : IntUartInterruptHandler

   Description : This is the interrupt handler for the internal uart. It
   determines which channel caused the interrupt before queueing any received
   chars and dequeueing chars waiting for transmission.
 ***************************************************************************/
static rtems_isr
IntUartInterruptHandler(rtems_vector_number v)
{
	unsigned int                chan = v - UART_INTC0_IRQ_VECTOR(0);
	struct IntUartInfoStruct   *info = &IntUartInfo[chan];

	/* check to see if received data */
	if ( MCF548X_PSC_ISR(chan) & MCF548X_PSC_ISR_RXRDY_FU )
	{
		/* read data and put into the receive buffer */
		while ( MCF548X_PSC_SR(chan) & MCF548X_PSC_SR_RXRDY )
		{

		   /* put data in rx buffer */
			info->rx_buffer[info->rx_in] = *((volatile uint8_t *)&MCF548X_PSC_RB(chan));

           /* check for errors */
           if ( MCF548X_PSC_SR(chan) & MCF548X_PSC_SR_ERROR )
			{
				/* clear the error */
				MCF548X_PSC_CR(chan) = MCF548X_PSC_CR_RESET_ERROR;
			}

			/* update buffer values */
			info->rx_in++;

			if ( info->rx_in >= RX_BUFFER_SIZE )
			{
				info->rx_in = 0;
			}
		}
		/* Make sure the port has been opened */
		if ( info->ttyp )
		{

			/* check to see if task driven */
			if ( info->iomode == TERMIOS_TASK_DRIVEN )
			{
				/* notify rx task that rx buffer has data */
				rtems_termios_rxirq_occured(info->ttyp);
			}
			else
			{
				/* Push up the received data */
				rtems_termios_enqueue_raw_characters(info->ttyp, info->rx_buffer, info->rx_in);
				info->rx_in    = 0;
			}
		}
	}

	/* check to see if data needs to be transmitted */
	if ( ( info->imr & MCF548X_PSC_IMR_TXRDY ) &&
		 ( MCF548X_PSC_ISR(chan) & MCF548X_PSC_ISR_TXRDY ) )
	{

		/* disable tx interrupts */
		info->imr &= ~MCF548X_PSC_IMR_TXRDY;
		MCF548X_PSC_IMR(chan) = info->imr;

		/* tell upper level that character has been sent */
		if ( info->ttyp )
			rtems_termios_dequeue_characters(info->ttyp, 1);
	}

}

/***************************************************************************
   Function : IntUartInitialize

   Description : This initialises the internal uart hardware for all
   internal uarts. If the internal uart is to be interrupt driven then the
   interrupt vectors are hooked.
 ***************************************************************************/
static void
IntUartInitialize(void)
{
    unsigned int        chan;
	struct IntUartInfoStruct   *info;
	rtems_isr_entry old_handler;
    int level;

	for ( chan = 0; chan < MAX_UART_INFO; chan++ )
	{
		info = &IntUartInfo[chan];

		info->ttyp     = NULL;
		info->rx_in    = 0;
		info->rx_out   = 0;
		info->baud     = -1;
		info->databits = -1;
		info->parity   = -1;
		info->stopbits = -1;
		info->hwflow   = -1;

		MCF548X_PSC_ACR(chan) = 0;
		MCF548X_PSC_IMR(chan) = 0;
		if ( info->iomode != TERMIOS_POLLED )
		{
			rtems_interrupt_catch (IntUartInterruptHandler,
								   UART_INTC0_IRQ_VECTOR(chan),
								   &old_handler);
		}

		/* set uart default values */
		IntUartSetAttributes(chan, NULL);

        /* unmask interrupt */
		rtems_interrupt_disable(level);
        switch(chan) {
        case 0:
            MCF548X_INTC_ICR35 =   MCF548X_INTC_ICRn_IL(PSC0_IRQ_LEVEL) |
                               MCF548X_INTC_ICRn_IP(PSC0_IRQ_PRIORITY);
            MCF548X_INTC_IMRH &= ~(MCF548X_INTC_IMRH_INT_MASK35);
            break;

        case 1:
            MCF548X_INTC_ICR34 =   MCF548X_INTC_ICRn_IL(PSC1_IRQ_LEVEL) |
                               MCF548X_INTC_ICRn_IP(PSC1_IRQ_PRIORITY);
            MCF548X_INTC_IMRH &= ~(MCF548X_INTC_IMRH_INT_MASK34);
            break;

        case 2:
            MCF548X_INTC_ICR33 =   MCF548X_INTC_ICRn_IL(PSC2_IRQ_LEVEL) |
                               MCF548X_INTC_ICRn_IP(PSC2_IRQ_PRIORITY);
            MCF548X_INTC_IMRH &= ~(MCF548X_INTC_IMRH_INT_MASK33);
            break;

        case 3:
            MCF548X_INTC_ICR32 =   MCF548X_INTC_ICRn_IL(PSC3_IRQ_LEVEL) |
                               MCF548X_INTC_ICRn_IP(PSC3_IRQ_PRIORITY);
            MCF548X_INTC_IMRH &= ~(MCF548X_INTC_IMRH_INT_MASK32);
            break;
        }
		rtems_interrupt_enable(level);

	} /* of chan loop */


} /* IntUartInitialise */

/***************************************************************************
   Function : IntUartInterruptWrite

   Description : This writes a single character to the appropriate uart
   channel. This is either called during an interrupt or in the user's task
   to initiate a transmit sequence. Calling this routine enables Tx
   interrupts.
 ***************************************************************************/
static ssize_t
IntUartInterruptWrite (int minor, const char *buf, size_t len)
{
	int level;

	rtems_interrupt_disable(level);

	/* write out character */
	*(volatile uint8_t *)(&MCF548X_PSC_TB(minor)) = *buf;

	/* enable tx interrupt */
	IntUartInfo[minor].imr |= MCF548X_PSC_IMR_TXRDY;
	MCF548X_PSC_IMR(minor) = IntUartInfo[minor].imr;

	rtems_interrupt_enable(level);
	return 0;
}

/***************************************************************************
   Function : IntUartInterruptOpen

   Description : This enables interrupts when the tty is opened.
 ***************************************************************************/
static int
IntUartInterruptOpen(int major, int minor, void *arg)
{
	struct IntUartInfoStruct   *info = &IntUartInfo[minor];

	/* enable the uart */
	MCF548X_PSC_CR(minor) = (MCF548X_PSC_CR_TX_ENABLED | MCF548X_PSC_CR_RX_ENABLED);

	/* check to see if interrupts need to be enabled */
	if ( info->iomode != TERMIOS_POLLED )
	{
		/* enable rx interrupts */
		info->imr |= MCF548X_PSC_IMR_RXRDY_FU;
		MCF548X_PSC_IMR(minor) = info->imr;
	}

	/* check to see if doing hardware flow control */
	if ( info->hwflow )
	{
		/* assert the RTS line */
		MCF548X_PSC_OPSET(minor) = MCF548X_PSC_OPSET_RTS;
	}

	return 0;
}


/***************************************************************************
   Function : IntUartInterruptClose

   Description : This disables interrupts when the tty is closed.
 ***************************************************************************/
static int
IntUartInterruptClose(int major, int minor, void *arg)
{
	struct IntUartInfoStruct   *info = &IntUartInfo[minor];

	/* disable the interrupts and the uart */
	MCF548X_PSC_IMR(minor) = 0;
	MCF548X_PSC_CR(minor) = (MCF548X_PSC_CR_TX_ENABLED | MCF548X_PSC_CR_RX_ENABLED);

	/* reset values */
	info->ttyp     = NULL;
	info->imr       = 0;
	info->rx_in    = 0;
	info->rx_out   = 0;

	return 0;
}

/***************************************************************************
   Function : IntUartTaskRead

   Description : This reads all available characters from the internal uart
   and places them into the termios buffer.  The rx interrupts will be
   re-enabled after all data has been read.
 ***************************************************************************/
static int
IntUartTaskRead(int minor)
{
	char                        buffer[RX_BUFFER_SIZE];
	int                         count;
	int                         rx_in;
	int                         index = 0;
	struct IntUartInfoStruct   *info  = &IntUartInfo[minor];

	/* determine number of values to copy out */
	rx_in = info->rx_in;
	if ( info->rx_out <= rx_in )
	{
		count = rx_in - info->rx_out;
	}
	else
	{
		count = (RX_BUFFER_SIZE - info->rx_out) + rx_in;
	}

	/* copy data into local buffer from rx buffer */
	while ( ( index < count ) && ( index < RX_BUFFER_SIZE ) )
	{
		/* copy data byte */
		buffer[index] = info->rx_buffer[info->rx_out];
		index++;

		/* increment rx buffer values */
		info->rx_out++;
		if ( info->rx_out >= RX_BUFFER_SIZE )
		{
			info->rx_out = 0;
		}
	}

	/* check to see if buffer is not empty */
	if ( count > 0 )
	{
		/* set characters into termios buffer  */
		rtems_termios_enqueue_raw_characters(info->ttyp, buffer, count);
	}

	return EOF;
}


/***************************************************************************
   Function : IntUartPollRead

   Description : This reads a character from the internal uart. It returns
   to the caller without blocking if not character is waiting.
 ***************************************************************************/
static int
IntUartPollRead (int minor)
{
if (!((MCF548X_PSC_SR(minor) & MCF548X_PSC_SR_RXRDY)))
		return(-1);

	return *((uint8_t *)&MCF548X_PSC_RB(minor));
}


/***************************************************************************
   Function : IntUartPollWrite

   Description : This writes out each character in the buffer to the
   appropriate internal uart channel waiting till each one is sucessfully
   transmitted.
 ***************************************************************************/
static ssize_t
IntUartPollWrite (int minor, const char *buf, size_t len)
{
	size_t retval = len;
/* loop over buffer */
	while ( len-- )
	{
		/* block until we can transmit */
		while (!((MCF548X_PSC_SR(minor) & MCF548X_PSC_SR_TXRDY)))
			continue;
		/* transmit data byte */
		*((uint8_t *)&MCF548X_PSC_TB(minor)) = *buf++;
	}
	return retval;
}

/***************************************************************************
   Function : console_initialize

   Description : This initialises termios, both sets of uart hardware before
   registering /dev/tty devices for each channel and the system /dev/console.
 ***************************************************************************/
rtems_device_driver console_initialize(
	rtems_device_major_number  major,
	rtems_device_minor_number  minor,
	void  *arg )
{
	rtems_status_code status;


	/* Set up TERMIOS */
	rtems_termios_initialize ();

	/* set io modes for the different channels and initialize device */
    IntUartInfo[minor].iomode = TERMIOS_IRQ_DRIVEN; //TERMIOS_POLLED;
	IntUartInitialize();

	/* Register the console port */
	status = rtems_io_register_name ("/dev/console", major, CONSOLE_PORT);
	if ( status != RTEMS_SUCCESSFUL )
	{
		rtems_fatal_error_occurred (status);
	}

	/* Register the other port */
	if ( CONSOLE_PORT != 0 )
	{
		status = rtems_io_register_name ("/dev/tty00", major, 0);
		if ( status != RTEMS_SUCCESSFUL )
		{
			rtems_fatal_error_occurred (status);
		}
	}
	if ( CONSOLE_PORT != 1 )
	{
		status = rtems_io_register_name ("/dev/tty01", major, 1);
		if ( status != RTEMS_SUCCESSFUL )
		{
			rtems_fatal_error_occurred (status);
		}
	}

	return(RTEMS_SUCCESSFUL);
}

/***************************************************************************
   Function : console_open

   Description : This actually opens the device depending on the minor
   number set during initialisation. The device specific access routines are
   passed to termios when the devices is opened depending on whether it is
   polled or not.
 ***************************************************************************/
rtems_device_driver console_open(
	rtems_device_major_number major,
	rtems_device_minor_number minor,
	void  * arg)
{
    rtems_status_code                status = RTEMS_INVALID_NUMBER;
	rtems_libio_open_close_args_t   *args   = (rtems_libio_open_close_args_t *)arg;
	struct IntUartInfoStruct        *info;

	static const rtems_termios_callbacks IntUartPollCallbacks = {
		NULL,				  /* firstOpen */
		NULL,				  /* lastClose */
		IntUartPollRead,	  /* pollRead */
		IntUartPollWrite,	  /* write */
		IntUartSetAttributes, /* setAttributes */
		NULL,				  /* stopRemoteTx */
		NULL,				  /* startRemoteTx */
		TERMIOS_POLLED		  /* mode */
	};
	static const rtems_termios_callbacks IntUartIntrCallbacks = {
		IntUartInterruptOpen,  /* firstOpen */
		IntUartInterruptClose, /* lastClose */
		NULL,				   /* pollRead */
		IntUartInterruptWrite, /* write */
		IntUartSetAttributes,  /* setAttributes */
		NULL,				   /* stopRemoteTx */
		NULL,				   /* startRemoteTx */
		TERMIOS_IRQ_DRIVEN	   /* mode */
	};

	static const rtems_termios_callbacks IntUartTaskCallbacks = {
		IntUartInterruptOpen,  /* firstOpen */
		IntUartInterruptClose, /* lastClose */
		IntUartTaskRead,	   /* pollRead */
		IntUartInterruptWrite, /* write */
		IntUartSetAttributes,  /* setAttributes */
		NULL,				   /* stopRemoteTx */
		NULL,				   /* startRemoteTx */
		TERMIOS_TASK_DRIVEN	   /* mode */
	};

	/* open the port depending on the minor device number */
	if ( ( minor >= 0 ) && ( minor < MAX_UART_INFO ) )
	{
		info = &IntUartInfo[minor];
		switch ( info->iomode )
		{
			case TERMIOS_POLLED:
				status = rtems_termios_open(major, minor, arg, &IntUartPollCallbacks);
				break;
			case TERMIOS_IRQ_DRIVEN:
				status = rtems_termios_open(major, minor, arg, &IntUartIntrCallbacks);
				info->ttyp = args->iop->data1;
				break;
			case TERMIOS_TASK_DRIVEN:
				status = rtems_termios_open(major, minor, arg, &IntUartTaskCallbacks);
				info->ttyp = args->iop->data1;
				break;
		}
	}

	return( status );
}

/***************************************************************************
   Function : console_close

   Description : This closes the device via termios
 ***************************************************************************/
rtems_device_driver console_close(
	rtems_device_major_number major,
	rtems_device_minor_number minor,
	void   * arg)
{
    return(rtems_termios_close (arg));
}

/***************************************************************************
   Function : console_read

   Description : Read from the device via termios
 ***************************************************************************/
rtems_device_driver console_read(
	rtems_device_major_number major,
	rtems_device_minor_number minor,
	void  * arg)
{
    return(rtems_termios_read (arg));
}

/***************************************************************************
   Function : console_write

   Description : Write to the device via termios
 ***************************************************************************/
rtems_device_driver console_write(
	rtems_device_major_number major,
	rtems_device_minor_number minor,
	void  * arg)
{
    return(rtems_termios_write (arg));
}

/***************************************************************************
   Function : console_ioctl

   Description : Pass the IOCtl call to termios
 ***************************************************************************/
rtems_device_driver console_control(
	rtems_device_major_number major,
	rtems_device_minor_number minor,
	void  * arg)
{
    return( rtems_termios_ioctl (arg) );
}


int DEBUG_OUTCHAR(int c)
{
    if(c == '\n')
        DEBUG_OUTCHAR('\r');
    _BSP_null_char(c);
    return c;
}
void DEBUG_OUTSTR(const char *msg)
{
    while (*msg)
        DEBUG_OUTCHAR(*msg++);
}
void DEBUG_OUTNUM(int i)
{
    int n;
    static const char map[] = "0123456789ABCDEF";
    DEBUG_OUTCHAR(' ');
    for (n = 28 ; n >= 0 ; n -= 4)
        DEBUG_OUTCHAR(map[(i >> n) & 0xF]);
}

