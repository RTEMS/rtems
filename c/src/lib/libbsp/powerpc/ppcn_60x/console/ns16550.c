/*
 *  This file contains the TTY driver for the NS16550
 *
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 * THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 * AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 * You are hereby granted permission to use, copy, modify, and distribute
 * this file, provided that this notice, plus the above copyright notice
 * and disclaimer, appears in all copies. Radstone Technology will provide
 * no support for this code.
 *
 *  This driver uses the termios pseudo driver.
 */

#include <rtems.h>
#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>

#include "console.h"
#include "ns16550_p.h"

/*
 * Flow control is only supported when using interrupts
 */
console_flow ns16550_flow_RTSCTS =
{
	ns16550_negate_RTS,		/* deviceStopRemoteTx */
	ns16550_assert_RTS		/* deviceStartRemoteTx */
};

console_flow ns16550_flow_DTRCTS =
{
	ns16550_negate_DTR,		/* deviceStopRemoteTx */
	ns16550_assert_DTR		/* deviceStartRemoteTx */
};

console_fns ns16550_fns =
{
	ns16550_probe,			/* deviceProbe */
	ns16550_open,			/* deviceFirstOpen */
	ns16550_flush,			/* deviceLastClose */
	NULL,				/* deviceRead */
	ns16550_write_support_int,	/* deviceWrite */
	ns16550_initialize_interrupts,	/* deviceInitialize */
	ns16550_write_polled,		/* deviceWritePolled */
	FALSE,				/* deviceOutputUsesInterrupts */
};

console_fns ns16550_fns_polled =
{
	ns16550_probe,			/* deviceProbe */
	ns16550_open,			/* deviceFirstOpen */
	ns16550_close,			/* deviceLastClose */
	ns16550_inbyte_nonblocking_polled,	/* deviceRead */
	ns16550_write_support_polled,	/* deviceWrite */
	ns16550_init,			/* deviceInitialize */
	ns16550_write_polled,		/* deviceWritePolled */
	FALSE,				/* deviceOutputUsesInterrupts */
};

/*
 *  Console Device Driver Entry Points
 */
static boolean ns16550_probe(int minor)
{
	/*
	 * If the configuration dependant probe has located the device then
	 * assume it is there
	 */
	return(TRUE);
}

static void ns16550_init(int minor)
{
	PSP_READ_REGISTERS	pNS16550Read;
	PSP_WRITE_REGISTERS	pNS16550Write;
	unsigned8	 	ucTrash;
	unsigned8	 	ucDataByte;
	unsigned32		ulBaudDivisor;
	ns16550_context		*pns16550Context;

	pns16550Context=(ns16550_context *)malloc(sizeof(ns16550_context));

	Console_Port_Data[minor].pDeviceContext=(void *)pns16550Context;
	pns16550Context->ucModemCtrl=SP_MODEM_IRQ;

	pNS16550Read=(PSP_READ_REGISTERS)Console_Port_Tbl[minor].ulCtrlPort1;
	pNS16550Write=(PSP_WRITE_REGISTERS)pNS16550Read;

	/* Clear the divisor latch, clear all interrupt enables,
	 * and reset and
	 * disable the FIFO's.
	 */

	outport_byte(&pNS16550Write->LineControl, 0x0);
	outport_byte(&pNS16550Write->InterruptEnable, 0x0);

	/* Set the divisor latch and set the baud rate. */

	ulBaudDivisor=NS16550_Baud(
		(unsigned32)Console_Port_Tbl[minor].pDeviceParams);
	ucDataByte = SP_LINE_DLAB;
	outport_byte(&pNS16550Write->LineControl, ucDataByte);
	outport_byte(&pNS16550Write->TransmitBuffer, ulBaudDivisor&0xff);
	outport_byte(&pNS16550Write->InterruptEnable, (ulBaudDivisor>>8)&0xff);

	/* Clear the divisor latch and set the character size to eight bits */
	/* with one stop bit and no parity checking. */

	ucDataByte = EIGHT_BITS;
	outport_byte(&pNS16550Write->LineControl, ucDataByte);

	/* Enable and reset transmit and receive FIFOs. TJA	*/
	ucDataByte = SP_FIFO_ENABLE;
	outport_byte(&pNS16550Write->FifoControl, ucDataByte);

	ucDataByte = SP_FIFO_ENABLE | SP_FIFO_RXRST | SP_FIFO_TXRST;
	outport_byte(&pNS16550Write->FifoControl, ucDataByte);

	/*
	 * Disable interrupts
	 */
	ucDataByte = 0;
	outport_byte(&pNS16550Write->InterruptEnable, ucDataByte);

	/* Set data terminal ready. */
	/* And open interrupt tristate line */

	outport_byte(&pNS16550Write->ModemControl,
		     pns16550Context->ucModemCtrl);

	inport_byte(&pNS16550Read->LineStatus, ucTrash);
	inport_byte(&pNS16550Read->ReceiveBuffer, ucTrash);
}

static int ns16550_open(
	int	 major,
	int	 minor,
	void	* arg
)
{
	PSP_WRITE_REGISTERS	pNS16550Write;

	pNS16550Write=(PSP_WRITE_REGISTERS)Console_Port_Tbl[minor].ulCtrlPort1;

	/*
	 * Assert DTR
	 */
	if(Console_Port_Tbl[minor].pDeviceFlow
	   !=&ns16550_flow_DTRCTS)
	{
		ns16550_assert_DTR(minor);
	}

	return(RTEMS_SUCCESSFUL);
}

static int ns16550_close(
	int	 major,
	int	 minor,
	void	* arg
)
{
	PSP_WRITE_REGISTERS	pNS16550Write;

	pNS16550Write=(PSP_WRITE_REGISTERS)Console_Port_Tbl[minor].ulCtrlPort1;

	/*
	 * Negate DTR
	 */
	if(Console_Port_Tbl[minor].pDeviceFlow
	   !=&ns16550_flow_DTRCTS)
	{
		ns16550_negate_DTR(minor);
	}

	return(RTEMS_SUCCESSFUL);
}

/* 
 *  ns16550_write_polled
 */
static void ns16550_write_polled(
	int   minor, 
	char  cChar
)
{
	PSP_READ_REGISTERS	pNS16550Read;
	PSP_WRITE_REGISTERS	pNS16550Write;
	unsigned char	ucLineStatus;
	int iTimeout;

	pNS16550Read=(PSP_READ_REGISTERS)Console_Port_Tbl[minor].ulCtrlPort1;
	pNS16550Write=(PSP_WRITE_REGISTERS)pNS16550Read;

	/*
	 * wait for transmitter holding register to be empty
	 */
	iTimeout=1000;
	inport_byte(&pNS16550Read->LineStatus, ucLineStatus);
	while ((ucLineStatus & SP_LSR_THOLD) == 0)
	{
		/*
		 * Yield while we wait
		 */
		if(_System_state_Is_up(_System_state_Get()))
		{
			rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
		}
		inport_byte(&pNS16550Read->LineStatus, ucLineStatus);
		if(!--iTimeout)
		{
			break;
		}
	}

	/*
	 * transmit character
	 */
	outport_byte(&pNS16550Write->TransmitBuffer, cChar);
}

/*
 * These routines provide control of the RTS and DTR lines
 */
/*
 *  ns16550_assert_RTS
 */
static void ns16550_assert_RTS(int minor)
{
	PSP_WRITE_REGISTERS	pNS16550Write;
	unsigned32 Irql;
	ns16550_context	*pns16550Context;

	pns16550Context=(ns16550_context *)
		       Console_Port_Data[minor].pDeviceContext;

	pNS16550Write=(PSP_WRITE_REGISTERS)Console_Port_Tbl[minor].ulCtrlPort1;

	/*
	 * Assert RTS
	 */
	rtems_interrupt_disable(Irql);
	pns16550Context->ucModemCtrl|=SP_MODEM_RTS;
	outport_byte(&pNS16550Write->ModemControl,
		     pns16550Context->ucModemCtrl);
	rtems_interrupt_enable(Irql);
}

/*
 *  ns16550_negate_RTS
 */
static void ns16550_negate_RTS(int minor)
{
	PSP_WRITE_REGISTERS	pNS16550Write;
	unsigned32 Irql;
	ns16550_context	*pns16550Context;

	pns16550Context=(ns16550_context *)
		       Console_Port_Data[minor].pDeviceContext;

	pNS16550Write=(PSP_WRITE_REGISTERS)Console_Port_Tbl[minor].ulCtrlPort1;

	/*
	 * Negate RTS
	 */
	rtems_interrupt_disable(Irql);
	pns16550Context->ucModemCtrl&=~SP_MODEM_RTS;
	outport_byte(&pNS16550Write->ModemControl,
		     pns16550Context->ucModemCtrl);
	rtems_interrupt_enable(Irql);
}

/*
 * These flow control routines utilise a connection from the local DTR
 * line to the remote CTS line
 */
/*
 *  ns16550_assert_DTR
 */
static void ns16550_assert_DTR(int minor)
{
	PSP_WRITE_REGISTERS	pNS16550Write;
	unsigned32 Irql;
	ns16550_context	*pns16550Context;

	pns16550Context=(ns16550_context *)
		       Console_Port_Data[minor].pDeviceContext;

	pNS16550Write=(PSP_WRITE_REGISTERS)Console_Port_Tbl[minor].ulCtrlPort1;

	/*
	 * Assert DTR
	 */
	rtems_interrupt_disable(Irql);
	pns16550Context->ucModemCtrl|=SP_MODEM_DTR;
	outport_byte(&pNS16550Write->ModemControl,
		     pns16550Context->ucModemCtrl);
	rtems_interrupt_enable(Irql);
}

/*
 *  ns16550_negate_DTR
 */
static void ns16550_negate_DTR(int minor)
{
	PSP_WRITE_REGISTERS	pNS16550Write;
	unsigned32 Irql;
	ns16550_context	*pns16550Context;

	pns16550Context=(ns16550_context *)
		       Console_Port_Data[minor].pDeviceContext;

	pNS16550Write=(PSP_WRITE_REGISTERS)Console_Port_Tbl[minor].ulCtrlPort1;

	/*
	 * Negate DTR
	 */
	rtems_interrupt_disable(Irql);
	pns16550Context->ucModemCtrl&=~SP_MODEM_DTR;
	outport_byte(&pNS16550Write->ModemControl,
		     pns16550Context->ucModemCtrl);
	rtems_interrupt_enable(Irql);
}

/*
 *  ns16550_isr
 *
 *  This routine is the console interrupt handler for COM1 and COM2
 *
 *  Input parameters:
 *    vector - vector number
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */

static void ns16550_process(
	int		minor
)
{
	PSP_READ_REGISTERS	pNS16550Read;
	PSP_WRITE_REGISTERS	pNS16550Write;
	volatile unsigned8 ucLineStatus, ucInterruptId;
	char	cChar;

	pNS16550Read=(PSP_READ_REGISTERS)Console_Port_Tbl[minor].ulCtrlPort1;
	pNS16550Write=(PSP_WRITE_REGISTERS)pNS16550Read;

	do
	{
		/*
		 * Deal with any received characters
		 */
		while(TRUE)
		{
			inport_byte(&pNS16550Read->LineStatus, ucLineStatus);
			if(~ucLineStatus & SP_LSR_RDY)
			{
				break;
			}
			inport_byte(&pNS16550Read->ReceiveBuffer, cChar);
			rtems_termios_enqueue_raw_characters(
				Console_Port_Data[minor].termios_data,
				&cChar, 1 );
		}

		while(TRUE)
		{
			if(Ring_buffer_Is_empty(
				&Console_Port_Data[minor].TxBuffer))
			{
				Console_Port_Data[minor].bActive=FALSE;
				if(Console_Port_Tbl[minor].pDeviceFlow
				   !=&ns16550_flow_RTSCTS)
				{
					ns16550_negate_RTS(minor);
				}
				/*
				 * There is no data to transmit
				 */
				break;
			}

			inport_byte(&pNS16550Read->LineStatus, ucLineStatus);
			if(~ucLineStatus & SP_LSR_THOLD)
			{
				/*
				 * We'll get another interrupt when
				 * the transmitter holding reg. becomes
				 * free again
				 */
				break;
			}

			Ring_buffer_Remove_character(
				&Console_Port_Data[minor].TxBuffer,
				cChar);
			/*
			 * transmit character
			 */
			outport_byte(&pNS16550Write->TransmitBuffer, cChar);
		}

		inport_byte(&pNS16550Read->InterruptId, ucInterruptId);
	}
	while((ucInterruptId&0xf)!=0x1);
}

static rtems_isr ns16550_isr(
  rtems_vector_number vector
)
{
	int	minor;

	for(minor=0;minor<Console_Port_Count;minor++)
	{
		if(vector==Console_Port_Tbl[minor].ulIntVector)
		{
			ns16550_process(minor);
		}
	}
}

/*
 *  ns16550_flush
 */
static int ns16550_flush(int major, int minor, void *arg)
{
	while(!Ring_buffer_Is_empty(&Console_Port_Data[minor].TxBuffer))
	{
		/*
		 * Yield while we wait
		 */
		if(_System_state_Is_up(_System_state_Get()))
		{
			rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
		}
	}

	ns16550_close(major, minor, arg);

	return(RTEMS_SUCCESSFUL);
}

/*
 *  ns16550_initialize_interrupts
 *
 *  This routine initializes the console's receive and transmit
 *  ring buffers and loads the appropriate vectors to handle the interrupts.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */

static void ns16550_enable_interrupts(
	int minor
)
{
	PSP_WRITE_REGISTERS	pNS16550Write;
	unsigned8	ucDataByte;

	pNS16550Write=(PSP_WRITE_REGISTERS)Console_Port_Tbl[minor].ulCtrlPort1;

	/*
	 * Enable interrupts
	 */
	ucDataByte = SP_INT_RX_ENABLE | SP_INT_TX_ENABLE;
	outport_byte(&pNS16550Write->InterruptEnable, ucDataByte);

}

static void ns16550_initialize_interrupts(int minor)
{
	ns16550_init(minor);

	Ring_buffer_Initialize(&Console_Port_Data[minor].TxBuffer);

	Console_Port_Data[minor].bActive = FALSE;

	set_vector(ns16550_isr,
		   Console_Port_Tbl[minor].ulIntVector,
		   1);

	ns16550_enable_interrupts(minor);
}

/* 
 *  ns16550_write_support_int
 *
 *  Console Termios output entry point.
 *
 */
static int ns16550_write_support_int(
  int   minor, 
  const char *buf, 
  int   len)
{
	int i;
	unsigned32 Irql;

	for(i=0; i<len;)
	{
		if(Ring_buffer_Is_full(&Console_Port_Data[minor].TxBuffer))
		{
			if(!Console_Port_Data[minor].bActive)
			{
				/*
				 * Wake up the device
				 */
				rtems_interrupt_disable(Irql);
				Console_Port_Data[minor].bActive = TRUE;
				if(Console_Port_Tbl[minor].pDeviceFlow
				   !=&ns16550_flow_RTSCTS)
				{
					ns16550_assert_RTS(minor);
				}
				ns16550_process(minor);
				rtems_interrupt_enable(Irql);
			}
			else
			{
				/*
				 * Yield
				 */
				rtems_task_wake_after(RTEMS_YIELD_PROCESSOR);
			}

			/*
			 * Wait for ring buffer to empty
			 */
			continue;
		}
		else
		{
			Ring_buffer_Add_character(
				&Console_Port_Data[minor].TxBuffer,
				buf[i]);
			i++;
		}
	}

	/*
	 * Ensure that characters are on the way
	 */
	if(!Console_Port_Data[minor].bActive)
	{
		/*
		 * Wake up the device
		 */
		rtems_interrupt_disable(Irql);
		Console_Port_Data[minor].bActive = TRUE;
		if(Console_Port_Tbl[minor].pDeviceFlow
		   !=&ns16550_flow_RTSCTS)
		{
			ns16550_assert_RTS(minor);
		}
		ns16550_process(minor);
		rtems_interrupt_enable(Irql);
	}

	return (len);
}

/* 
 *  ns16550_write_support_polled
 *
 *  Console Termios output entry point.
 *
 */
static int ns16550_write_support_polled(
  int   minor, 
  const char *buf, 
  int   len)
{
	int nwrite = 0;

	/*
	 * poll each byte in the string out of the port.
	 */
	while (nwrite < len)
	{
		/*
		 * transmit character
		 */
		ns16550_write_polled(minor, *buf++);
		nwrite++;
	}

	/*
	 * return the number of bytes written.
	 */
	return nwrite;
}

/* 
 *  ns16550_inbyte_nonblocking_polled 
 *
 *  Console Termios polling input entry point.
 */

static int ns16550_inbyte_nonblocking_polled( 
	int minor 
)
{
	PSP_READ_REGISTERS	pNS16550Read;
	unsigned char	ucLineStatus;
	char	cChar;

	pNS16550Read=(PSP_READ_REGISTERS)Console_Port_Tbl[minor].ulCtrlPort1;

	inport_byte(&pNS16550Read->LineStatus, ucLineStatus);
	if(ucLineStatus & SP_LSR_RDY)
	{
		inport_byte(&pNS16550Read->ReceiveBuffer, cChar);
		return((int)cChar);
	}
	else
	{
	return(-1);
	}
}
