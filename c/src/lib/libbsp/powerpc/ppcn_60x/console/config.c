/*
 *  This file contains the TTY driver table for the PPCn_60x
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
 *  $Id$
 */

#include <libchip/serial.h>
#include <libchip/mc68681.h>
#include <libchip/z85c30.h>
#include "i8042vga.h"

#include "ns16550cfg.h"
#include "z85c30cfg.h"

#include <pci.h>

#define PMX1553_BUS	2
#define PMX1553_SLOT	1

/*
 *  Based on BSP configuration information decide whether to do polling IO
 *  or interrupt driven IO.
 */

#if (CONSOLE_USE_INTERRUPTS)
#define NS16550_FUNCTIONS &ns16550_fns
#define Z85C30_FUNCTIONS  &z85c30_fns
#else
#define NS16550_FUNCTIONS &ns16550_fns_polled
#define Z85C30_FUNCTIONS  &z85c30_fns_polled
#endif

/*
 * Configuration specific probe routines
 */
static boolean config_PMX1553_probe(int minor);
static boolean config_z85c30_probe(int minor);

/*
 * The following table configures the console drivers used in this BSP.
 *
 * The first entry which, when probed, is available, will be named /dev/console,
 * all others being given the name indicated.
 *
 * Each field is interpreted thus:
 *
 * sDeviceName	This is the name of the device.
 * pDeviceFns	This is a pointer to the set of driver routines to use.
 * pDeviceFlow	This is a pointer to the set of flow control routines to
 *		use. Serial device drivers will typically supply RTSCTS
 *		and DTRCTS handshake routines for DCE to DCE communication,
 *		however for DCE to DTE communication, no such routines
 *		should be necessary as RTS will be driven automatically
 *		when the transmitter is active.
 * ulMargin	The high water mark in the input buffer is set to the buffer
 *		size less ulMargin. Once this level is reached, the driver's
 *		flow control routine used to stop the remote transmitter will
 *		be called. This figure should be greater than or equal to
 *		the number of stages of FIFO between the transmitter and
 *		receiver.
 * ulHysteresis	After the high water mark specified by ulMargin has been
 *		reached, the driver's routine to re-start the remote
 *		transmitter will be called once the level in the input
 *		buffer has fallen by ulHysteresis bytes.
 * pDeviceParams This contains either device specific data or a pointer to a
 *		device specific structure containing additional information
 *		not provided in this table.
 * ulCtrlPort1	This is the primary control port number for the device. This
 *		may be used to specify different instances of the same device
 *		type.
 * ulCtrlPort2	This is the secondary control port number, of use when a given
 *		device has more than one available channel.
 * ulDataPort	This is the port number for the data port of the device
 * ulIntVector	This encodes the interrupt vector of the device.
 *
 */
console_tbl	Console_Port_Tbl[] = {
	{
		"/dev/vga",			/* sDeviceName */
		SERIAL_CUSTOM,			/* deviceType */
		&i8042vga_fns,			/* pDeviceFns */
		NULL,				/* deviceProbe */
		NULL,				/* pDeviceFlow */
		0,				/* ulMargin */
		0,				/* ulHysteresis */
		(void *)0,			/* pDeviceParams */
		I8042_CS,			/* ulCtrlPort1 */
		0,				/* ulCtrlPort2 */
		I8042_DATA,			/* ulDataPort */
		Read_ns16550_register,		/* getRegister */
		Write_ns16550_register,		/* setRegister */
		NULL,				/* getData */
		NULL,				/* setData */
		0,				/* ulClock */
		PPCN_60X_IRQ_KBD		/* ulIntVector */
	},
	{
		"/dev/com1",			/* sDeviceName */
		SERIAL_NS16550,			/* deviceType */
		NS16550_FUNCTIONS,		/* pDeviceFns */
		NULL,				/* deviceProbe */
		&ns16550_flow_RTSCTS,		/* pDeviceFlow */
		16,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
		NS16550_PORT_A,			/* ulCtrlPort1 */
		0,				/* ulCtrlPort2 */
		NS16550_PORT_A,			/* ulDataPort */
		Read_ns16550_register,		/* getRegister */
		Write_ns16550_register,		/* setRegister */
		NULL,				/* getData */
		NULL,				/* setData */
		0,				/* ulClock */
		PPCN_60X_IRQ_COM1		/* ulIntVector */
	},
	{
		"/dev/ser1",			/* sDeviceName */
		SERIAL_NS16550,			/* deviceType */
		NS16550_FUNCTIONS,		/* pDeviceFns */
		config_PMX1553_probe,		/* deviceProbe */
		&ns16550_flow_RTSCTS,		/* pDeviceFlow */
		80,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
		PMX1553_BUS,	/* PCI bus */	/* ulCtrlPort1 */
		PMX1553_SLOT,	/* PCI slot */	/* ulCtrlPort2 */
		1,	/* Channel 1-4 */	/* ulDataPort */
		NULL,				/* getRegister */
		NULL,				/* setRegister */
		NULL,				/* getData */
		NULL,				/* setData */
		0,				/* ulClock */
		0	/* RS232 */		/* ulIntVector */
	},
	{
		"/dev/ser2",			/* sDeviceName */
		SERIAL_NS16550,			/* deviceType */
		NS16550_FUNCTIONS,		/* pDeviceFns */
		config_PMX1553_probe,		/* deviceProbe */
		&ns16550_flow_RTSCTS,		/* pDeviceFlow */
		80,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
		PMX1553_BUS,	/* PCI bus */	/* ulCtrlPort1 */
		PMX1553_SLOT,	/* PCI slot */	/* ulCtrlPort2 */
		2,	/* Channel 1-4 */	/* ulDataPort */
		Read_ns16550_register,		/* getRegister */
		Write_ns16550_register,		/* setRegister */
		NULL,				/* getData */
		NULL,				/* setData */
		0,				/* ulClock */
		0	/* RS232 */		/* ulIntVector */
	},
	{
		"/dev/ser3",			/* sDeviceName */
		SERIAL_NS16550,			/* deviceType */
		NS16550_FUNCTIONS,		/* pDeviceFns */
		config_PMX1553_probe,		/* deviceProbe */
		&ns16550_flow_RTSCTS,		/* pDeviceFlow */
		96,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)57600,	/* baud rate */	/* pDeviceParams */
		PMX1553_BUS,	/* PCI bus */	/* ulCtrlPort1 */
		PMX1553_SLOT,	/* PCI slot */	/* ulCtrlPort2 */
		3,	/* Channel 1-4 */	/* ulDataPort */
		Read_ns16550_register,		/* getRegister */
		Write_ns16550_register,		/* setRegister */
		NULL,				/* getData */
		NULL,				/* setData */
		0,				/* ulClock */
		0	/* RS232 */		/* ulIntVector */
	},
	{
		"/dev/ser4",			/* sDeviceName */
		SERIAL_NS16550,			/* deviceType */
		NS16550_FUNCTIONS,		/* pDeviceFns */
		config_PMX1553_probe,		/* deviceProbe */
		&ns16550_flow_RTSCTS,		/* pDeviceFlow */
		96,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)57600,	/* baud rate */	/* pDeviceParams */
		PMX1553_BUS,	/* PCI bus */	/* ulCtrlPort1 */
		PMX1553_SLOT,	/* PCI slot */	/* ulCtrlPort2 */
		4,	/* Channel 1-4 */	/* ulDataPort */
		Read_ns16550_register,		/* getRegister */
		Write_ns16550_register,		/* setRegister */
		NULL,				/* getData */
		NULL,				/* setData */
		0,				/* ulClock */
		0	/* RS232 */		/* ulIntVector */
	},
#if !PPCN_60X_USE_DINK
	{
		"/dev/com2",			/* sDeviceName */
		SERIAL_NS16550,			/* deviceType */
		NS16550_FUNCTIONS,		/* pDeviceFns */
		NULL,				/* deviceProbe */
		&ns16550_flow_RTSCTS,		/* pDeviceFlow */
		16,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
		NS16550_PORT_B,			/* ulCtrlPort1 */
		0,				/* ulCtrlPort2 */
		NS16550_PORT_B,			/* ulDataPort */
		Read_ns16550_register,		/* getRegister */
		Write_ns16550_register,		/* setRegister */
		NULL,				/* getData */
		NULL,				/* setData */
		0,				/* ulClock */
		PPCN_60X_IRQ_COM2		/* ulIntVector */
	},
#endif
	{
		"/dev/com3",			/* sDeviceName */
		SERIAL_Z85C30,			/* deviceType */
		Z85C30_FUNCTIONS,		/* pDeviceFns */
		config_z85c30_probe,		/* deviceProbe */
		&z85c30_flow_RTSCTS,		/* pDeviceFlow */
		16,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
		Z85C30_CTRL_A,			/* ulCtrlPort1 */
		Z85C30_CTRL_A,			/* ulCtrlPort2 */
		Z85C30_DATA_A,			/* ulDataPort */
		Read_85c30_register,		/* getRegister */
		Write_85c30_register,		/* setRegister */
		Read_85c30_data,		/* getData */
		Write_85c30_data,		/* setData */
		0,				/* ulClock */
		PPCN_60X_IRQ_COM3_4		/* ulIntVector */
	},
	{
		"/dev/com4",			/* sDeviceName */
		SERIAL_Z85C30,			/* deviceType */
		Z85C30_FUNCTIONS,		/* pDeviceFns */
		config_z85c30_probe,		/* deviceProbe */
		&z85c30_flow_RTSCTS,		/* pDeviceFlow */
		16,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
		Z85C30_CTRL_B,			/* ulCtrlPort1 */
		Z85C30_CTRL_A,			/* ulCtrlPort2 */
		Z85C30_DATA_B,			/* ulDataPort */
		Read_85c30_register,		/* getRegister */
		Write_85c30_register,		/* setRegister */
		Read_85c30_data,		/* getData */
		Write_85c30_data,		/* setData */
		0,				/* ulClock */
		PPCN_60X_IRQ_COM3_4		/* ulIntVector */
	}
};

/*
 * Define serial port write registers structure.
 */
typedef volatile struct _SP_WRITE_REGISTERS {
    unsigned char TransmitBuffer;
    unsigned char InterruptEnable;
    unsigned char FifoControl;
    unsigned char LineControl;
    unsigned char ModemControl;
    unsigned char Reserved1;
    unsigned char ModemStatus;
    unsigned char ScratchPad;
} SP_WRITE_REGISTERS, *PSP_WRITE_REGISTERS;

static boolean config_PMX1553_probe(int minor)
{
	unsigned8 ucBusNumber, ucSlotNumber, ucChannel;
	unsigned8 ucIntLine;
	unsigned32 ulPortBase, ulMemBase, ulDeviceID;
	unsigned8 *pucSIO_cir, *pucUart_int_sr, *pucUartDevIntReg;
	PSP_WRITE_REGISTERS     pNS16550Write;

	/*
	 * Extract PCI bus/slot and channel number
	 */
	ucBusNumber=Console_Port_Tbl[minor].ulCtrlPort1;
	ucSlotNumber=Console_Port_Tbl[minor].ulCtrlPort2;
	ucChannel=Console_Port_Tbl[minor].ulDataPort;

	PCIConfigRead32(ucBusNumber,
			ucSlotNumber,
			0,
			PCI_CONFIG_VENDOR_LOW,
			&ulDeviceID);

	if(ulDeviceID!=0x000111b5)
	{
		return FALSE;
	}

	/*
	 * At this point we know we have a PMC1553 or PMX1553 card
	 *
	 * Check for PMX1553 uart legacy IO ports
	 */
	PCIConfigRead32(ucBusNumber,
			ucSlotNumber,
			0,
			PCI_CONFIG_BAR_3,
			&ulPortBase);

	if(ulPortBase==0)
	{
		/*
		 * This is either a PMC1553 or we can't see the uart
		 * registers
		 */
		return FALSE;
	}

	PCIConfigRead32(ucBusNumber,
			ucSlotNumber,
			0,
			PCI_CONFIG_BAR_2,
			&ulMemBase);

	pucUartDevIntReg=(unsigned8 *)(PCI_MEM_BASE+ulMemBase);
	pucUart_int_sr=(unsigned8 *)(PCI_MEM_BASE+ulMemBase+0x10);
	pucSIO_cir=(unsigned8 *)(PCI_MEM_BASE+ulMemBase+0x18);

	/*
	 * Use ulIntVector field to select RS232/RS422
	 */
	if(Console_Port_Tbl[minor].ulIntVector==0)
	{
		/*
		 * Select RS232 mode
		 */
		*pucSIO_cir&=~(1<<(ucChannel-1));
	}
	else
	{
		/*
		 * Select RS422 mode
		 */
		*pucSIO_cir|=1<<(ucChannel-1);
	}
	EIEIO;
	/*
	 * Bring device out of reset
	 */
	*pucSIO_cir&=0xbf;
	EIEIO;
	/*
	 * Enable all channels as active
	 */
	*pucSIO_cir|=0x10;
	EIEIO;
	*pucSIO_cir&=0xdf;

	PCIConfigRead8(ucBusNumber,
		       ucSlotNumber,
		       0,
		       PCI_CONFIG_INTERRUPTLINE,
		       &ucIntLine);

	ulPortBase&=~PCI_ADDRESS_IO_SPACE;

	ulPortBase+=8*(ucChannel-1);

	Console_Port_Tbl[minor].ulCtrlPort1=
	Console_Port_Tbl[minor].ulDataPort=ulPortBase;
	if(Console_Port_Tbl[minor].pDeviceFns!=&ns16550_fns_polled)
	{
		Console_Port_Tbl[minor].ulIntVector=PPCN_60X_IRQ_PCI(ucIntLine);

		/*
		 * Enable interrupt
		 */
		*pucUart_int_sr=(~*pucUart_int_sr)&(0x08<<ucChannel);

		/*
		 * Enable interrupt to PCI
		 */
		*pucUartDevIntReg=(~*pucUartDevIntReg)&0x80;
	}
	else
	{
		/*
		 * Disable interrupt
		 */
		*pucUart_int_sr&=(0x08<<ucChannel);
	}

	/*
	 * Enable Auto CTS to facilitate flow control
	 */
	pNS16550Write=(PSP_WRITE_REGISTERS)Console_Port_Tbl[minor].ulCtrlPort1;
	/*
	 * Enable special register set and unlock Enhanced Feature Register
	 */
	outport_byte(&pNS16550Write->LineControl, 0xbf);
	/*
	 * Unlock enhanced function bits
	 */
	outport_byte(&pNS16550Write->FifoControl, 0x10);
	/*
	 * Disable special register set and lock Enhanced Feature Register
	 */
	outport_byte(&pNS16550Write->LineControl, 0);
	/*
	 * Select div 1
	 */
	outport_byte(&pNS16550Write->ModemControl, 0x00);
	/*
	 * Enable special register set and unlock Enhanced Feature Register
	 */
	outport_byte(&pNS16550Write->LineControl, 0xbf);
	/*
	 * Lock enhanced function bits and enable auto CTS
	 */
	outport_byte(&pNS16550Write->FifoControl, 0x80);
	/*
	 * Disable special register set and lock Enhanced Feature Register
	 */
	outport_byte(&pNS16550Write->LineControl, 0);
	
	/*
	 * The PMX1553 currently uses a 16 MHz clock rather than the
	 * 7.3728 MHz clock described in the ST16C654 data sheet. When
	 * available, 22.1184 MHz will be used allowing rates up to
	 * 1382400 baud (RS422 only).
	 */
#if 1
	/*
	 * Scale requested baud rate for 16 MHz clock
	 */
	(unsigned32)Console_Port_Tbl[minor].pDeviceParams*=7373;
	(unsigned32)Console_Port_Tbl[minor].pDeviceParams/=16000;
#else
	/*
	 * Scale requested baud rate for 22.1184 MHz clock
	 */
	(unsigned32)Console_Port_Tbl[minor].pDeviceParams/=3;
#endif
	/*
	 * In order to maintain maximum data rate accuracy, we will
	 * apply a div 4 here rather than in hardware (using MCR bit 7).
	 */
	(unsigned32)Console_Port_Tbl[minor].pDeviceParams/=4;

	return(TRUE);
}

static boolean config_z85c30_probe(int minor)
{
	/*
	 * PPC1 and PPC1a do not have this device
	 */
	if((ucSystemType==SYS_TYPE_PPC1) ||
	   (ucSystemType==SYS_TYPE_PPC1a))
	{
		return (FALSE);
	}

	/*
	 * All other boards supported by this BSP have the z85c30 device
	 */

	/*
	 * Ensure that CIO port B is configured for
	 * default driver enable
	 */
	outport_byte(0x861, 0x33);

	return(TRUE);
}

