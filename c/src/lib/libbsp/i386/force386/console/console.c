/*
 *  This file contains the Force CPU386 console IO package.
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

#define F386_INIT

#include <stdlib.h>

#include <rtems.h>
#include "console.h"
#include "bsp.h"

/*  console_cleanup
 *
 *  This routine is called at exit to clean up the console hardware.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

void console_cleanup( void )
{
  register rtems_unsigned8 ignored;
  /*
   *  FORCE technical support mentioned that it may be necessary to
   *  read the DUSCC RX_BUFFER port four times to remove all junk.
   *  This code is a little more paranoid.
   */

   inport_byte( RX_BUFFER, ignored );
   inport_byte( RX_BUFFER, ignored );
   inport_byte( RX_BUFFER, ignored );
   inport_byte( RX_BUFFER, ignored );
   inport_byte( RX_BUFFER, ignored );
}

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
   /*
    *  flush the console now and at exit.  Just in case.
    */

   console_cleanup();

   atexit( console_cleanup );
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
  register rtems_unsigned8 status;

  inport_byte( RX_STATUS, status );

  if ( Is_rx_ready( status ) ) {
     inport_byte( RX_BUFFER,  status );
     *ch = status;
     return TRUE;
  }
  return FALSE;
}

/*  inbyte
 *
 *  This routine reads a character from the UART.
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
  register rtems_unsigned8 status;
  char                     ch;

  do {
    inport_byte( RX_STATUS, status );
  } while ( !Is_rx_ready( status ) );

#if ( PORTB == 1 )
  /*
   *  Force example code resets the Channel B Receiver here.
   *  It appears to cause XON's to be lost.
   */

   /* outport_byte( RX_STATUS, 0x10 );  */
#endif

  inport_byte( RX_BUFFER, ch );

  return ch;
}

/*  outbyte
 *
 *  This routine transmits a character out the port.  It supports
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
  rtems_unsigned8 status;

  do {
    inport_byte( TX_STATUS, status );
  } while ( !Is_tx_ready( status ) );

#if 0
  while ( is_character_ready( &status ) == TRUE ) {      /* must be an XOFF */
    if ( status == XOFF )
      do {
        while ( is_character_ready( &status ) == FALSE ) ;
      } while ( status != XON );
  }
#endif

  outport_byte( TX_BUFFER, ch );
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
