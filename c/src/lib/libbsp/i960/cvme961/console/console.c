/*
 *  This file contains the MVME136 console IO package.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#define C961_INIT

#include <rtems.h>
#include "console.h"
#include "bsp.h"

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
  *status = RTEMS_SUCCESSFUL;
}

/*
 *  NINDY_IO( ... )
 *
 *  Interface to NINDY.
 */

#define NINDY_INPUT   0
#define NINDY_OUTPUT  1

void NINDY_IO();

void ___NINDY_IO_WRAPPER( void )  /* never called */
{
   asm volatile ( "       .text" );
   asm volatile ( "       .align 4" );
   asm volatile ( "       .globl _NINDY_IO" );
   asm volatile ( "_NINDY_IO:" );
   asm volatile ( "        calls   0       /* call console routines */" );
   asm volatile ( "        ret" );
}

/*  inbyte
 *
 *  This routine reads a character from the console using NINDY.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    character read from UART
 */

char inbyte( void )
{
  char ch;

  NINDY_IO( NINDY_INPUT, &ch );
  return ch;
}


/*  outbyte
 *
 *  This routine transmits a character out the console using NINDY.
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
  NINDY_IO( NINDY_OUTPUT, ch );
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
