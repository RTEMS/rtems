/*
 *  This file contains the MVME162 console IO package.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  Modifications of respective RTEMS file: COPYRIGHT (c) 1994.
 *  EISCAT Scientific Association. M.Savitski
 *
 *  This material is a part of the MVME162 Board Support Package
 *  for the RTEMS executive. Its licensing policies are those of the
 *  RTEMS above.
 *
 *  $Id$
 */

#define M162_INIT

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
  rtems_unsigned8 rr_0;

  Z8x30_READ_CONTROL( CONSOLE_CONTROL, RR_0, rr_0 );
  if ( !(rr_0 & RR_0_RX_DATA_AVAILABLE) )
    return( FALSE );

  Z8x30_READ_DATA( CONSOLE_DATA, *ch );

  return(TRUE);
}

/*  inbyte
 *
 *  This routine reads a character from the SCC.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    character read from SCC
 */

char inbyte( void )
{
  rtems_unsigned8 rr_0;
  char ch;

  while ( 1 ) {
    Z8x30_READ_CONTROL( CONSOLE_CONTROL, RR_0, rr_0 );
    if ( (rr_0 & RR_0_RX_DATA_AVAILABLE) != 0 )
      break;
  }

  Z8x30_READ_DATA( CONSOLE_DATA, ch );
  return ch;
}


/*  outbyte
 *
 *  This routine transmits a character out the SCC.  It supports
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
  rtems_unsigned8 rr_0;
  char            flow_control;

  while ( 1 ) {
    Z8x30_READ_CONTROL( CONSOLE_CONTROL, RR_0, rr_0 );
    if ( (rr_0 & RR_0_TX_BUFFER_EMPTY) != 0 )
      break;
  }

  while ( 1 ) {
    Z8x30_READ_CONTROL( CONSOLE_CONTROL, RR_0, rr_0 );
    if ( (rr_0 & RR_0_RX_DATA_AVAILABLE) == 0 )
      break;

    Z8x30_READ_DATA( CONSOLE_DATA, flow_control );

    if ( flow_control == XOFF )
      do {
        do {
          Z8x30_READ_CONTROL( CONSOLE_CONTROL, RR_0, rr_0 );
        } while ( (rr_0 & RR_0_RX_DATA_AVAILABLE) == 0 );
        Z8x30_READ_DATA( CONSOLE_DATA, flow_control );
      } while ( flow_control != XON );
  }

  Z8x30_WRITE_DATA( CONSOLE_DATA, ch );
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
