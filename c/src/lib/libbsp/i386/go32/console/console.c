/*
 *  This file contains the go32 console IO package.
 *
 *  $Id$
 */

#define IBMPC_INIT

#include <stdlib.h>

#include <rtems.h>
#include "console.h"
#include "bsp.h"

#include <dpmi.h>
#include <go32.h>

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
    /* nothing */
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

/* Set this if console I/O should use go32 (DOS) read/write calls.	*/
/* Otherwise, direct hardware accesses will be used.			*/
int			_IBMPC_Use_Go32_IO	= 0;

static rtems_isr_entry	old_keyboard_isr	= NULL;
extern void		_IBMPC_keyboard_isr( rtems_unsigned32 interrupt );


rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg,
  rtems_id                   self,
  rtems_unsigned32          *status
)
{
    if ( _IBMPC_Use_Go32_IO )  {
	/* Nothing.  We let DOS and go32 do all the work. */
    } else {
	/* Grap the keyboard interrupt so DOS doesn't steal our */
	/* keystrokes.						*/
	rtems_status_code	status;
	status = rtems_interrupt_catch( _IBMPC_keyboard_isr, 9,
				        &old_keyboard_isr );
	if ( status )  {
	    int write( int, void *, int );
	    void exit( int );
	    char msg[] = "error initializing keyboard\n";
	    write( 2, msg, sizeof msg - 1 );
	    exit( 1 );
	}
    }

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
    return _IBMPC_chrdy( ch ) ? TRUE : FALSE;
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
    char ch = _IBMPC_inch();
#if 1
    /* Echo character to screen */
    void outbyte( char ch );
    outbyte( ch );
    if ( ch == '\r' )
	outbyte( '\n' );
#endif
    return ch;
}

/*  outbyte
 *
 *  This routine transmits a character out the port.
 *
 *  Input parameters:
 *    ch  - character to be transmitted
 *
 *  Output parameters:  NONE
 */

void outbyte( char ch )
{
    _IBMPC_outch( ch );
}

/*
 * __read  -- read bytes from the console. Ignore fd, since
 *            we only have stdin.
 */

int __read(
  int fd,
  char *buf,
  int nbytes
)
{
  int i = 0;

  for ( i = 0; i < nbytes; i++ ) {
    buf[i] = inbyte();
    if ( buf[i] == '\r' ) {
	/* What if this goes past the end of the buffer?  We're hosed. [bhc] */
	buf[i++] = '\n';
	buf[i] = '\0';
	break;
    }
  }
  return i;
}

/*
 * __write -- write bytes to the console. Ignore fd, since
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
