/*
 *  This file contains the TTY driver table for the DY-4 DMV177.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994, 1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  $Id$
 */

#include <bsp.h>

#include <libchip/serial.h>
#include <libchip/mc68681.h>
#include <libchip/z85c30.h>

/*
 * Configuration specific probe routines
 *
 *
 * NOTE:  There are no DMV177 specific configuration routines.  These
 *        routines could be written to dynamically determine which serial
 *        ports are on a board.  Debugging this would require access to
 *        multiple board models.
 */

/* NONE CURRENTLY PROVIDED */

/*
 * The following table configures the console drivers used in this BSP.
 *
 * The first entry which, when probed, is available, will be named /dev/console,
 * all others being given the name indicated.
 */

mc68681_baud_t 
  dmv177_mc68681_baud_table[4][RTEMS_TERMIOS_NUMBER_BAUD_RATES] = {
  { /* ACR[7] = 0, X = 0 */
    MC68681_BAUD_NOT_VALID,    /* B0 */
    MC68681_BAUD_NOT_VALID,    /* B50 */
    0x01,                      /* B75 */
    MC68681_BAUD_NOT_VALID,    /* B110 */
    0x03,                      /* B134 */
    MC68681_BAUD_NOT_VALID,    /* B150 */
    0x04,                      /* B200 */
    MC68681_BAUD_NOT_VALID,    /* B300 */
    MC68681_BAUD_NOT_VALID,    /* B600 */
    MC68681_BAUD_NOT_VALID,    /* B1200 */
    MC68681_BAUD_NOT_VALID,    /* B1800 */
    MC68681_BAUD_NOT_VALID,    /* B2400 */
    0x0A,                      /* B4800 */
    MC68681_BAUD_NOT_VALID,    /* B9600 */
    MC68681_BAUD_NOT_VALID,    /* B19200 */
    MC68681_BAUD_NOT_VALID,    /* B38400 */
    MC68681_BAUD_NOT_VALID,    /* B57600 */
    MC68681_BAUD_NOT_VALID,    /* B115200 */
    MC68681_BAUD_NOT_VALID,    /* B230400 */
    MC68681_BAUD_NOT_VALID     /* B460800 */
  },
  { /* ACR[7] = 1, X = 0 */
    MC68681_BAUD_NOT_VALID,    /* B0 */
    0x00,                      /* B50 */
    0x01,                      /* B75 */
    MC68681_BAUD_NOT_VALID,    /* B110 */
    MC68681_BAUD_NOT_VALID,    /* B134 */
    MC68681_BAUD_NOT_VALID,    /* B150 */
    0x04,                      /* B200 */
    MC68681_BAUD_NOT_VALID,    /* B300 */
    MC68681_BAUD_NOT_VALID,    /* B600 */
    0x0A,                      /* B1200 */
    MC68681_BAUD_NOT_VALID,    /* B1800 */
    MC68681_BAUD_NOT_VALID,    /* B2400 */
    MC68681_BAUD_NOT_VALID,    /* B4800 */
    MC68681_BAUD_NOT_VALID,    /* B9600 */
    MC68681_BAUD_NOT_VALID,    /* B19200 */
    MC68681_BAUD_NOT_VALID,    /* B38400 */
    MC68681_BAUD_NOT_VALID,    /* B57600 */
    MC68681_BAUD_NOT_VALID,    /* B115200 */
    MC68681_BAUD_NOT_VALID,    /* B230400 */
    MC68681_BAUD_NOT_VALID     /* B460800 */
  },
  { /* ACR[7] = 0, X = 1 */
    MC68681_BAUD_NOT_VALID,    /* B0 */
    0x00,                      /* B50 */
    0x01,                      /* B75 */
    MC68681_BAUD_NOT_VALID,    /* B110 */
    MC68681_BAUD_NOT_VALID,    /* B134 */
    MC68681_BAUD_NOT_VALID,    /* B150 */
    MC68681_BAUD_NOT_VALID,    /* B200 */
    MC68681_BAUD_NOT_VALID,    /* B300 */
    MC68681_BAUD_NOT_VALID,    /* B600 */
    MC68681_BAUD_NOT_VALID,    /* B1200 */
    MC68681_BAUD_NOT_VALID,    /* B1800 */
    0x04,                      /* B2400 */
    MC68681_BAUD_NOT_VALID,    /* B4800 */
    0x05,                      /* B9600 */
    0x06,                      /* B19200 */
    0x07,                      /* B38400 */
    MC68681_BAUD_NOT_VALID,    /* B57600 */
    MC68681_BAUD_NOT_VALID,    /* B115200 */
    MC68681_BAUD_NOT_VALID,    /* B230400 */
    MC68681_BAUD_NOT_VALID     /* B460800 */
  },
  { /* ACR[7] = 1, X = 1 */
    MC68681_BAUD_NOT_VALID,    /* B0 */
    MC68681_BAUD_NOT_VALID,    /* B50 */
    0x01,                      /* B75 */
    MC68681_BAUD_NOT_VALID,    /* B110 */
    0x03,                      /* B134 */
    MC68681_BAUD_NOT_VALID,    /* B150 */
    MC68681_BAUD_NOT_VALID,    /* B200 */
    MC68681_BAUD_NOT_VALID,    /* B300 */
    MC68681_BAUD_NOT_VALID,    /* B600 */
    MC68681_BAUD_NOT_VALID,    /* B1200 */
    MC68681_BAUD_NOT_VALID,    /* B1800 */
    0x04,                      /* B2400 */
    0x0A,                      /* B4800 */
    0x05,                      /* B9600 */
    0x06,                      /* B19200 */
    0x07,                      /* B38400 */
    MC68681_BAUD_NOT_VALID,    /* B57600 */
    MC68681_BAUD_NOT_VALID,    /* B115200 */
    MC68681_BAUD_NOT_VALID,    /* B230400 */
    MC68681_BAUD_NOT_VALID     /* B460800 */
  },
};

#define MC68681_PORT_CONFIG  \
  (MC68681_DATA_BAUD_RATE_SET_1|MC68681_XBRG_ENABLED) 

/*
 *  Based on BSP configuration information decide whether to do polling IO
 *  or interrupt driven IO.
 */

#if (CONSOLE_USE_INTERRUPTS)
#define MC68681_FUNCTIONS &mc68681_fns
#define Z85C30_FUNCTIONS  &z85c30_fns
#else
#define MC68681_FUNCTIONS &mc68681_fns_polled
#define Z85C30_FUNCTIONS  &z85c30_fns_polled
#endif

boolean dmv177_z85c30_probe(int minor);
boolean dmv177_mc68681_probe(int minor);

console_tbl	Console_Port_Tbl[] = {
	{
		"/dev/com0",			/* sDeviceName */
                SERIAL_MC68681,                 /* deviceType */
		MC68681_FUNCTIONS,		/* pDeviceFns */
		dmv177_mc68681_probe,		/* deviceProbe */
		NULL,				/* pDeviceFlow */
		16,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
		MC68681_ADDR,			/* ulCtrlPort1 */
		MC68681_PORT1_ADDR,		/* ulCtrlPort2 */
		MC68681_PORT_CONFIG,		/* ulDataPort */
		mc68681_get_register_8,         /* getRegister */
		mc68681_set_register_8,		/* setRegister */
		NULL, /* unused */		/* getData */
		NULL, /* unused */		/* setData */
                (unsigned32)dmv177_mc68681_baud_table, /* ulClock */
		DMV170_DUART_IRQ		/* ulIntVector */
	},
	{
		"/dev/com1",			/* sDeviceName */
                SERIAL_MC68681,                 /* deviceType */
		MC68681_FUNCTIONS,		/* pDeviceFns */
		dmv177_mc68681_probe,		/* deviceProbe */
		NULL,				/* pDeviceFlow */
		16,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
		MC68681_ADDR,			/* ulCtrlPort1 */
		MC68681_PORT2_ADDR,		/* ulCtrlPort2 */
		MC68681_PORT_CONFIG,		/* ulDataPort */
		mc68681_get_register_8,         /* getRegister */
		mc68681_set_register_8,		/* setRegister */
		NULL, /* unused */		/* getData */
		NULL, /* unused */		/* setData */
                (unsigned32)dmv177_mc68681_baud_table, /* ulClock */
		DMV170_DUART_IRQ		/* ulIntVector */
	},
	{
		"/dev/com3",			/* sDeviceName */
                SERIAL_Z85C30,                  /* deviceType */
		Z85C30_FUNCTIONS,		/* pDeviceFns */
		dmv177_z85c30_probe,		/* deviceProbe */
		NULL,				/* pDeviceFlow */
		16,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
		Z85C30_CTRL_A,			/* ulCtrlPort1 */
		Z85C30_CTRL_A,			/* ulCtrlPort2 */
		0,				/* ulDataPort */
		z85c30_get_register,		/* getRegister */
		z85c30_set_register,		/* setRegister */
		NULL,				/* getData */
		NULL,				/* setData */
                0, /* filled in by probe */     /* ulClock */
		DMV170_SCC_IRQ			/* ulIntVector */
	},
	{
		"/dev/com4",			/* sDeviceName */
                SERIAL_Z85C30,                  /* deviceType */
		Z85C30_FUNCTIONS,		/* pDeviceFns */
		dmv177_z85c30_probe,		/* deviceProbe */
		NULL,				/* pDeviceFlow */
		16,				/* ulMargin */
		8,				/* ulHysteresis */
		(void *)9600,	/* baud rate */	/* pDeviceParams */
		Z85C30_CTRL_B,			/* ulCtrlPort1 */
		Z85C30_CTRL_A,			/* ulCtrlPort2 */
		0,				/* ulDataPort */
		z85c30_get_register,		/* getRegister */
		z85c30_set_register,		/* setRegister */
		NULL,				/* getData */
		NULL,				/* setData */
                0, /* filled in by probe */     /* ulClock */
		DMV170_SCC_IRQ			/* ulIntVector */
	}
};

/*
 *  Declare some information used by the console driver
 */

#define NUM_CONSOLE_PORTS (sizeof(Console_Port_Tbl)/sizeof(console_tbl))

unsigned long  Console_Port_Count = NUM_CONSOLE_PORTS;

console_data  Console_Port_Data[NUM_CONSOLE_PORTS];

rtems_device_minor_number  Console_Port_Minor;

/*
 *  Hopefully, by checking the card resource register, this BSP
 *  will be able to operate on the DMV171, DMV176, or DMV177.
 */

boolean dmv177_z85c30_probe(int minor)
{
  volatile unsigned32 *dma_control_status_reg;
  volatile unsigned16 *card_resource_reg;
  unsigned16 v;

  card_resource_reg = (volatile unsigned16 *) DMV170_CARD_RESORCE_REG;

  v = *card_resource_reg & DMV170_SCC_INST_MASK;

  if ( v != DMV170_SCC_INSTALLED )
    return FALSE;

  /*
   *  Figure out the clock speed of the Z85C30 SCC
   */

  dma_control_status_reg = (volatile unsigned32 *)DMV170_DMA_CONTROL_STATUS_REG;

  if ( *dma_control_status_reg & DMV170_SCC_10MHZ )
    Console_Port_Tbl[minor].ulClock = Z85C30_CLOCK_10;
  else
    Console_Port_Tbl[minor].ulClock = Z85C30_CLOCK_2;

  return TRUE;
}

boolean dmv177_mc68681_probe(int minor)
{
  volatile unsigned16 *card_resource_reg;
  unsigned16 v;

  card_resource_reg = (volatile unsigned16 *) DMV170_CARD_RESORCE_REG;

  v = *card_resource_reg & DMV170_DUART_INST_MASK;

  if ( v == DMV170_DUART_INSTALLED )
    return TRUE;

  return FALSE;
}
