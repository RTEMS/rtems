/*  
 *  This file contains the Motorola IDP console IO package.
 *
 *  Written by Doug McBride, Colorado Space Grant College
 *  Based off of the board support packages of RTEMS
 *
 *  Updated to RTEMS 3.2.0 by Joel Sherrill.
 *
 *  $Id$
 */

#define MIDP_INIT

#include "rtems.h"
#include "console.h"
#include "bsp.h"

#include "ringbuf.h"

Ring_buffer_t  Buffer[ 2 ];

rtems_isr C_Receive_ISR(rtems_vector_number vector);


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

  Ring_buffer_Initialize( &Buffer[ 0 ] );
  Ring_buffer_Initialize( &Buffer[ 1 ] );

  init_pit();

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
  char *ch,
  int   port
)
{
  if ( Ring_buffer_Is_empty( &Buffer[ port ] ) )
    return FALSE;

  Ring_buffer_Remove_character( &Buffer[ port ], *ch );
  return TRUE;
}

/*  quick_char_check
 *
 *  This routine returns TRUE if a character is available.
 *  It is different from above because it does not disturb the ring buffer
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 */

rtems_boolean quick_char_check( 
  int   port
)
{
  if ( Ring_buffer_Is_empty( &Buffer[ port ] ) )
    return FALSE;

  return TRUE;
}

/*  inbyte
 *
 *  This routine reads a character from the UART through a buffer.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    character read from UART
 */

char inbyte( 
  int port
)
{
  unsigned char tmp_char;
 
  /* If you come into this routine without checking is_character_ready() first
     and you want nonblocking code, then it's your own fault */

  while ( !is_character_ready( &tmp_char, port ) );
 
  return tmp_char;
}


/*  outbyte
 *
 *  This routine transmits a character out the M68681.  It supports
 *  XON/XOFF flow control.
 *
 *  Input parameters:
 *    ch  - character to be transmitted 
 *
 *  Output parameters:  NONE
 */

void outbyte( 
  char ch,
  int  port
)
{
  switch ( port ) {
    case 0:
      transmit_char( ch );
      break;
    case 1:
      transmit_char_portb( ch );
      break;
  }

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
  int port;

  /*
   *  Map port A to stdin, stdout, and stderr.
   *  Map everything else to port B.
   */

  if ( fd <= 2 ) port = 0;
  else           port = 1;

  for (i = 0; i < nbytes; i++) {
    *(buf + i) = inbyte( port );
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
  int port;
 
  /*
   *  Map port A to stdin, stdout, and stderr.
   *  Map everything else to port B.
   */
 
  if ( fd <= 2 ) port = 0;
  else           port = 1;
 
  for (i = 0; i < nbytes; i++) {
    if (*(buf + i) == '\n') {
      outbyte ('\r', port );
    }
    outbyte (*(buf + i), port );
  }
  return (nbytes);
}
