/*
 *  This file contains the template for a console IO package.
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

#define NO_BSP_INIT

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
  /*
   *  If polling, wait until a character is available.
   */

  return '\0';
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
  /*
   *  If polling, wait for the transmitter to be ready.
   *  Check for flow control requests and process.
   *  Then output the character.
   */

  /*
   *  Carriage Return/New line translation.
   */

  if ( ch == '\n' )
    outbyte( '\r' );
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
