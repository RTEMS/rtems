/*
 *  This file contains the TTY driver table for the EP1A
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <libchip/serial.h>
#include <libchip/ns16550.h>
#include "ns16550cfg.h"
#include <bsp.h>
#include <libcpu/io.h>
#include "m68360.h"

/*
 *  Based on BSP configuration information decide whether to do polling IO
 *  or interrupt driven IO.
 */
#define NS16550_FUNCTIONS &ns16550_fns_polled
#define MC68360_SCC_FUNCTIONS &mc68360_scc_fns

/*
 * Configuration specific probe routines
 */
static bool config_68360_scc_base_probe_1(int minor);
static bool config_68360_scc_base_probe_2(int minor);
static bool config_68360_scc_base_probe_3(int minor);
static bool config_68360_scc_base_probe_4(int minor);
static bool config_68360_scc_base_probe_5(int minor);
static bool config_68360_scc_base_probe_6(int minor);
static bool config_68360_scc_base_probe_7(int minor);
static bool config_68360_scc_base_probe_8(int minor);
static bool config_68360_scc_base_probe_9(int minor);
static bool config_68360_scc_base_probe_10(int minor);
static bool config_68360_scc_base_probe_11(int minor);
static bool config_68360_scc_base_probe_12(int minor);

extern console_fns mc68360_scc_fns;

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
console_tbl	Console_Configuration_Ports[] = {
        /*
         *  NS16550 Chips provide first COM1 and COM2 Ports.
         */
	{
		"/dev/com1",			/* sDeviceName */
		SERIAL_NS16550,			/* deviceType */
		NS16550_FUNCTIONS,		/* pDeviceFns */
		NULL,				/* deviceProbe */
		&ns16550_flow_RTSCTS,		/* pDeviceFlow */
		16,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
                UART_BASE_COM1,                 /* ulCtrlPort1 */
		0,				/* ulCtrlPort2 */
                UART_BASE_COM1,       		/* ulDataPort */
		Read_ns16550_register,		/* getRegister */
		Write_ns16550_register,		/* setRegister */
		NULL,				/* getData */
		NULL,				/* setData */
		7372800,			/* ulClock */
                0				/* ulIntVector */
	},
        {
                "/dev/com2",                    /* sDeviceName */
                SERIAL_NS16550,                 /* deviceType */
                NS16550_FUNCTIONS,              /* pDeviceFns */
                NULL,                           /* deviceProbe */
                &ns16550_flow_RTSCTS,           /* pDeviceFlow */
                16,                             /* ulMargin */
                8,                              /* ulHysteresis */
                (void *)9600,   /* baud rate */ /* pDeviceParams */
                UART_BASE_COM2,                  /* ulCtrlPort1-Filled in at runtime */
                0,                              /* ulCtrlPort2 */
                UART_BASE_COM2,                 /* ulDataPort-Filled in at runtime*/
                Read_ns16550_register,          /* getRegister */
                Write_ns16550_register,         /* setRegister */
                NULL,                           /* getData */
                NULL,                           /* setData */
                7372800,                        /* ulClock */
                0                               /* ulIntVector */
        },
        /*
         * Up to 12 serial ports are provided by MC68360 SCC ports.
         *     EP1A may have one MC68360 providing 4 ports (A,B,C,D).
         */
        {
                "/dev/rs232_2",                 /* sDeviceName */
                SERIAL_CUSTOM,                  /* deviceType */
                MC68360_SCC_FUNCTIONS,          /* pDeviceFns */
                config_68360_scc_base_probe_1,  /* deviceProbe */
                NULL,                           /* pDeviceFlow */
                16,                             /* ulMargin */
                8,                              /* ulHysteresis */
                (void *)9600,   /* baud rate */ /* pDeviceParams */
                0,                              /* ulCtrlPort1 */
                0,                              /* ulCtrlPort2 */
                0,                              /* ulDataPort */
                NULL,                           /* getRegister */
                NULL,                           /* setRegister */
                NULL,                           /* getData */
                NULL,                           /* setData */
                0,                              /* ulClock */
                0                               /* ulIntVector */
        },
        {
                "/dev/rs422_1",                 /* sDeviceName */
                SERIAL_CUSTOM,                  /* deviceType */
                MC68360_SCC_FUNCTIONS,          /* pDeviceFns */
                config_68360_scc_base_probe_2,  /* deviceProbe */
                NULL,                           /* pDeviceFlow */
                16,                             /* ulMargin */
                8,                              /* ulHysteresis */
                (void *)9600,   /* baud rate */ /* pDeviceParams */
                0,                              /* ulCtrlPort1 */
                0,                              /* ulCtrlPort2 */
                0,                              /* ulDataPort */
                NULL,                           /* getRegister */
                NULL,                           /* setRegister */
                NULL,                           /* getData */
                NULL,                           /* setData */
                0,                              /* ulClock */
                0                               /* ulIntVector */
        },
        {
                "/dev/rs422_2",                    /* sDeviceName */
                SERIAL_CUSTOM,                  /* deviceType */
                MC68360_SCC_FUNCTIONS,          /* pDeviceFns */
                config_68360_scc_base_probe_3,   /* deviceProbe */
                NULL,                           /* pDeviceFlow */
                16,                             /* ulMargin */
                8,                              /* ulHysteresis */
                (void *)9600,   /* baud rate */ /* pDeviceParams */
                0,                              /* ulCtrlPort1 */
                0,                              /* ulCtrlPort2 */
                0,                              /* ulDataPort */
                NULL,                           /* getRegister */
                NULL,                           /* setRegister */
                NULL,                           /* getData */
                NULL,                           /* setData */
                0,                              /* ulClock */
                0                               /* ulIntVector */
        },
        {
                "/dev/ep1a_d",                  /* sDeviceName */
                SERIAL_CUSTOM,                  /* deviceType */
                MC68360_SCC_FUNCTIONS,          /* pDeviceFns */
                config_68360_scc_base_probe_4,  /* deviceProbe */
                NULL,                           /* pDeviceFlow */
                16,                             /* ulMargin */
                8,                              /* ulHysteresis */
                (void *)9600,   /* baud rate */ /* pDeviceParams */
                0,                              /* ulCtrlPort1 */
                0,                              /* ulCtrlPort2 */
                0,                              /* ulDataPort */
                NULL,                           /* getRegister */
                NULL,                           /* setRegister */
                NULL,                           /* getData */
                NULL,                           /* setData */
                0,                              /* ulClock */
                0                               /* ulIntVector */
        },
        /*
         * PMC1 may have one MC16550 providing 4 ports (A,B,C,D).
         */
        {
                "/dev/rs422_3",                 /* sDeviceName */
                SERIAL_CUSTOM,                  /* deviceType */
                MC68360_SCC_FUNCTIONS,          /* pDeviceFns */
                config_68360_scc_base_probe_5,  /* deviceProbe */
                NULL,                           /* pDeviceFlow */
                16,                             /* ulMargin */
                8,                              /* ulHysteresis */
                (void *)9600,   /* baud rate */ /* pDeviceParams */
                0,                              /* ulCtrlPort1 */
                0,                              /* ulCtrlPort2 */
                0,                              /* ulDataPort */
                NULL,                           /* getRegister */
                NULL,                           /* setRegister */
                NULL,                           /* getData */
                NULL,                           /* setData */
                0,                              /* ulClock */
                0                               /* ulIntVector */
        },
        {
                "/dev/rs422_4",                 /* sDeviceName */
                SERIAL_CUSTOM,                  /* deviceType */
                MC68360_SCC_FUNCTIONS,          /* pDeviceFns */
                config_68360_scc_base_probe_6,  /* deviceProbe */
                NULL,                           /* pDeviceFlow */
                16,                             /* ulMargin */
                8,                              /* ulHysteresis */
                (void *)9600,   /* baud rate */ /* pDeviceParams */
                0,                              /* ulCtrlPort1 */
                0,                              /* ulCtrlPort2 */
                0,                              /* ulDataPort */
                NULL,                           /* getRegister */
                NULL,                           /* setRegister */
                NULL,                           /* getData */
                NULL,                           /* setData */
                0,                              /* ulClock */
                0                               /* ulIntVector */
        },
        {
                "/dev/rs422_5",                 /* sDeviceName */
                SERIAL_CUSTOM,                  /* deviceType */
                MC68360_SCC_FUNCTIONS,          /* pDeviceFns */
                config_68360_scc_base_probe_7,  /* deviceProbe */
                NULL,                           /* pDeviceFlow */
                16,                             /* ulMargin */
                8,                              /* ulHysteresis */
                (void *)9600,   /* baud rate */ /* pDeviceParams */
                0,                              /* ulCtrlPort1 */
                0,                              /* ulCtrlPort2 */
                0,                              /* ulDataPort */
                NULL,                           /* getRegister */
                NULL,                           /* setRegister */
                NULL,                           /* getData */
                NULL,                           /* setData */
                0,                              /* ulClock */
                0                               /* ulIntVector */
        },
        {
                "/dev/rs422_6",                 /* sDeviceName */
                SERIAL_CUSTOM,                  /* deviceType */
                MC68360_SCC_FUNCTIONS,          /* pDeviceFns */
                config_68360_scc_base_probe_8,  /* deviceProbe */
                NULL,                           /* pDeviceFlow */
                16,                             /* ulMargin */
                8,                              /* ulHysteresis */
                (void *)9600,   /* baud rate */ /* pDeviceParams */
                0,                              /* ulCtrlPort1 */
                0,                              /* ulCtrlPort2 */
                0,                              /* ulDataPort */
                NULL,                           /* getRegister */
                NULL,                           /* setRegister */
                NULL,                           /* getData */
                NULL,                           /* setData */
                0,                              /* ulClock */
                0                               /* ulIntVector */
        },
        /*
         * PMC2 may have one MC16550 providing 4 ports (A,B,C,D).
         */
        {
                "/dev/rs232_3",                 /* sDeviceName */
                SERIAL_CUSTOM,                  /* deviceType */
                MC68360_SCC_FUNCTIONS,          /* pDeviceFns */
                config_68360_scc_base_probe_9,  /* deviceProbe */
                NULL,                           /* pDeviceFlow */
                16,                             /* ulMargin */
                8,                              /* ulHysteresis */
                (void *)9600,   /* baud rate */ /* pDeviceParams */
                0,                              /* ulCtrlPort1 */
                0,                              /* ulCtrlPort2 */
                0,                              /* ulDataPort */
                NULL,                           /* getRegister */
                NULL,                           /* setRegister */
                NULL,                           /* getData */
                NULL,                           /* setData */
                0,                              /* ulClock */
                0                               /* ulIntVector */
        },
        {
                "/dev/rs232_4",                 /* sDeviceName */
                SERIAL_CUSTOM,                  /* deviceType */
                MC68360_SCC_FUNCTIONS,          /* pDeviceFns */
                config_68360_scc_base_probe_10, /* deviceProbe */
                NULL,                           /* pDeviceFlow */
                16,                             /* ulMargin */
                8,                              /* ulHysteresis */
                (void *)9600,   /* baud rate */ /* pDeviceParams */
                0,                              /* ulCtrlPort1 */
                0,                              /* ulCtrlPort2 */
                0,                              /* ulDataPort */
                NULL,                           /* getRegister */
                NULL,                           /* setRegister */
                NULL,                           /* getData */
                NULL,                           /* setData */
                0,                              /* ulClock */
                0                               /* ulIntVector */
        },
        {
                "/dev/rs232_5",                 /* sDeviceName */
                SERIAL_CUSTOM,                  /* deviceType */
                MC68360_SCC_FUNCTIONS,          /* pDeviceFns */
                config_68360_scc_base_probe_11, /* deviceProbe */
                NULL,                           /* pDeviceFlow */
                16,                             /* ulMargin */
                8,                              /* ulHysteresis */
                (void *)9600,   /* baud rate */ /* pDeviceParams */
                0,                              /* ulCtrlPort1 */
                0,                              /* ulCtrlPort2 */
                0,                              /* ulDataPort */
                NULL,                           /* getRegister */
                NULL,                           /* setRegister */
                NULL,                           /* getData */
                NULL,                           /* setData */
                0,                              /* ulClock */
                0                               /* ulIntVector */
        },
        {
                "/dev/rs232_6",                 /* sDeviceName */
                SERIAL_CUSTOM,                  /* deviceType */
                MC68360_SCC_FUNCTIONS,          /* pDeviceFns */
                config_68360_scc_base_probe_12, /* deviceProbe */
                NULL,                           /* pDeviceFlow */
                16,                             /* ulMargin */
                8,                              /* ulHysteresis */
                (void *)9600,   /* baud rate */ /* pDeviceParams */
                0,                              /* ulCtrlPort1 */
                0,                              /* ulCtrlPort2 */
                0,                              /* ulDataPort */
                NULL,                           /* getRegister */
                NULL,                           /* setRegister */
                NULL,                           /* getData */
                NULL,                           /* setData */
                0,                              /* ulClock */
                0                               /* ulIntVector */
        }
};

/* 
 *  Define a variable that contains the number of statically configured
 *  console devices.
 */
unsigned long  Console_Configuration_Count = \
    (sizeof(Console_Configuration_Ports)/sizeof(console_tbl));

static bool config_68360_scc_base_probe(int minor, unsigned long busNo, unsigned long slotNo, int channel)
{
  M68360_t chip = M68360_chips;

  /*
   * Find out if the chip is installed.
   */
  while (chip) {
    if ((chip->board_data->slotNo == slotNo) && (chip->board_data->busNo == busNo))
      break;
    chip = chip->next;
  }

  if (!chip)
    return false;

  Console_Port_Tbl[minor]->pDeviceParams = &chip->port[ channel-1 ];
  chip->port[ channel-1 ].minor         = minor;
  return true;
}

static bool config_68360_scc_base_probe_1( int minor ) {
  return config_68360_scc_base_probe(minor, 0, 11, 1);
}

static bool config_68360_scc_base_probe_2( int minor ) {
  return config_68360_scc_base_probe(minor, 0, 11, 2);
}

static bool config_68360_scc_base_probe_3( int minor ) {
  return config_68360_scc_base_probe( minor, 0, 11, 3);
}

static bool config_68360_scc_base_probe_4( int minor ) {
  return config_68360_scc_base_probe( minor, 0, 11, 4);
}

static bool config_68360_scc_base_probe_5( int minor ) {
  return config_68360_scc_base_probe( minor, 0, 16, 1);
}

static bool config_68360_scc_base_probe_6( int minor ) {
  return config_68360_scc_base_probe( minor, 0, 16, 2);
}

static bool config_68360_scc_base_probe_7( int minor ) {
  return config_68360_scc_base_probe( minor, 0, 16, 3);
}

static bool config_68360_scc_base_probe_8( int minor ) {
  return config_68360_scc_base_probe( minor, 0, 16, 4);
}

static bool config_68360_scc_base_probe_9( int minor ) {
  return config_68360_scc_base_probe( minor, 0, 15, 1);
}

static bool config_68360_scc_base_probe_10( int minor ) {
  return config_68360_scc_base_probe( minor, 0, 15, 2);
}

static bool config_68360_scc_base_probe_11( int minor ) {
  return config_68360_scc_base_probe( minor, 0, 15, 3);
}

static bool config_68360_scc_base_probe_12( int minor ) {
  return config_68360_scc_base_probe( minor, 0, 15, 4);
}

