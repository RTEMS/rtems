/*
 *  This file contains the PowerPC 403GA console IO package.
 *
 *  Author:	Andrew Bray <andy@i-cubed.demon.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/lib/libbsp/no_cpu/no_bsp/console/console.c:
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  console.c,v 1.2 1995/05/31 16:56:07 joel Exp
 */

#define NO_BSP_INIT

#include <rtems.h>
#include "console.h"
#include "bsp.h"

extern rtems_cpu_table           Cpu_table;             /* owned by BSP */

struct async {
/*-----------------------------------------------------------------------------+
| Line Status Register.
+-----------------------------------------------------------------------------*/
    unsigned char SPLS;
    unsigned char SPLSset;
#define LSRDataReady             0x80
#define LSRFramingError          0x40
#define LSROverrunError          0x20
#define LSRParityError           0x10
#define LSRBreakInterrupt        0x08
#define LSRTxHoldEmpty           0x04
#define LSRTxShiftEmpty          0x02

/*-----------------------------------------------------------------------------+
| Handshake Status Register.
+-----------------------------------------------------------------------------*/
    unsigned char SPHS;
    unsigned char SPHSset;
#define HSRDsr                   0x80
#define HSRCts                   0x40

/*-----------------------------------------------------------------------------+
| Baud rate divisor registers
+-----------------------------------------------------------------------------*/
    unsigned char BRDH;
    unsigned char BRDL;

/*-----------------------------------------------------------------------------+
| Control Register.
+-----------------------------------------------------------------------------*/
    unsigned char SPCTL;
#define CRNormal		      0x00
#define CRLoopback		      0x40
#define CRAutoEcho		      0x80
#define CRDtr                    0x20
#define CRRts                    0x10
#define CRWordLength7            0x00
#define CRWordLength8            0x08
#define CRParityDisable          0x00
#define CRParityEnable           0x04
#define CREvenParity             0x00
#define CROddParity	      0x02
#define CRStopBitsOne            0x00
#define CRStopBitsTwo            0x01
#define CRDisableDtrRts	      0x00

/*-----------------------------------------------------------------------------+
| Receiver Command Register.
+-----------------------------------------------------------------------------*/
    unsigned char SPRC;
#define RCRDisable	              0x00
#define RCREnable		      0x80
#define RCRIntDisable	      0x00
#define RCRIntEnabled	      0x20
#define RCRDMACh2		      0x40
#define RCRDMACh3	              0x60
#define RCRErrorInt	      0x10
#define RCRPauseEnable	      0x08

/*-----------------------------------------------------------------------------+
| Transmitter Command Register.
+-----------------------------------------------------------------------------*/
    unsigned char SPTC;
#define TCRDisable	              0x00
#define TCREnable		      0x80
#define TCRIntDisable	      0x00
#define TCRIntEnabled	      0x20
#define TCRDMACh2		      0x40
#define TCRDMACh3	              0x60
#define TCRTxEmpty		      0x10
#define TCRErrorInt	      0x08
#define TCRStopPause	      0x04
#define TCRBreakGen	      0x02

/*-----------------------------------------------------------------------------+
| Miscellanies defines.
+-----------------------------------------------------------------------------*/
    unsigned char SPTB;
#define SPRB	SPTB
};

#define XOFFchar  		      0x13
#define XONchar  		      0x11

typedef volatile struct async *pasync;
static const pasync port = (pasync)0x40000000;

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

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg,
  rtems_id                   self,
  rtems_unsigned32          *status
)
{
  register unsigned tmp;

  /* Initialise the serial port */
  asm volatile ("mfiocr %0" : "=r" (tmp));
  tmp &= ~3;
  tmp |= (Cpu_table.serial_external_clock ? 2 : 0) |
      (Cpu_table.serial_cts_rts ? 1 : 0);
  asm volatile ("mtiocr %0" : "=r" (tmp) : "0" (tmp));
  port->SPLS = (LSRDataReady | LSRFramingError | LSROverrunError |
	       LSRParityError | LSRBreakInterrupt);
  tmp = Cpu_table.serial_per_sec / Cpu_table.serial_rate;
  tmp = ((tmp + 8) >> 4) - 1;
  port->BRDL = tmp & 0x255;
  port->BRDH = tmp >> 8;
  port->SPCTL = (CRNormal | CRDtr | CRRts | CRWordLength8 | CRParityDisable |
		 CRStopBitsOne);
  port->SPRC = (RCREnable | RCRIntDisable | RCRPauseEnable);
  port->SPTC = (TCREnable | TCRIntDisable);
  port->SPHS = (HSRDsr | HSRCts);

  *status = RTEMS_SUCCESSFUL;
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
  unsigned char status;

  if ((status = port->SPLS) & LSRDataReady)
    {
      *ch = port->SPRB;  
      return(TRUE);
    }

  /* Clean any dodgy status */
  if ((status & (LSRFramingError | LSROverrunError | LSRParityError |
		 LSRBreakInterrupt)) != 0)
    {
      port->SPLS = (LSRFramingError | LSROverrunError | LSRParityError |
		 LSRBreakInterrupt);
    }

  return FALSE;
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
  unsigned char status;

  while (1)
    {
      if ((status = port->SPLS) & LSRDataReady)
	break;

      /* Clean any dodgy status */
      if ((status & (LSRFramingError | LSROverrunError | LSRParityError |
		     LSRBreakInterrupt)) != 0)
	{
	  port->SPLS = (LSRFramingError | LSROverrunError | LSRParityError |
			LSRBreakInterrupt);
	}
    }

  return port->SPRB;  
}

/*  outbyte
 *
 *  This routine transmits a character out the SOURCE.  It may support
 *  XON/XOFF flow control.
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
  unsigned char status;

  while (port->SPHS)
    port->SPHS = (HSRDsr | HSRCts);

  while (1)
    {
      status = port->SPLS;

      if (port->SPHS)
        port->SPHS = (HSRDsr | HSRCts);
      else if (status & LSRTxHoldEmpty)
	break;
    }

  if (Cpu_table.serial_xon_xoff)
    while (is_character_ready(&status))
      {
	if (status == XOFFchar)
	  do
	    {
	      while (!is_character_ready(&status));
	    }
	  while (status != XONchar);
      }

  port->SPTB = ch;
}

/*
 * __read  -- read bytes from the serial port. Ignore fd, since
 *            we only have stdin.
 */

int __read(
  int fd,
  char *buf,
  int nbytes
)
{
  int i = 0;

  for (i = 0; i < nbytes; i++) {
    *(buf + i) = inbyte();
    if ((*(buf + i) == '\n') || (*(buf + i) == '\r')) {
      (*(buf + i++)) = '\n';
      (*(buf + i)) = 0;
      break;
    }
  }
  return (i);
}

/*
 * __write -- write bytes to the serial port. Ignore fd, since
 *            stdout and stderr are the same. Since we have no filesystem,
 *            open will only return an error.
 */

int __write(
  int fd,
  char *buf,
  int nbytes
)
{
  int i;

  for (i = 0; i < nbytes; i++) {
    if (*(buf + i) == '\n') {
      outbyte ('\r');
    }
    outbyte (*(buf + i));
  }
  return (nbytes);
}
