/*
 *  This file contains the go32 console IO package.
 *
 *  $Id$
 */

#define IBMPC_INIT

#include <stdlib.h>

#include <bsp.h>
#include <rtems/libio.h>

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

/* Set this if console I/O should use go32 (DOS) read/write calls.   */
/* Otherwise, direct hardware accesses will be used.                 */

int      _IBMPC_Use_Go32_IO  = 0;

static rtems_isr_entry  old_keyboard_isr  = NULL;

extern void    _IBMPC_keyboard_isr( rtems_unsigned32 interrupt );

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;

  if ( _IBMPC_Use_Go32_IO )  {
    /* Nothing.  We let DOS and go32 do all the work. */
  } else {
    /* Grap the keyboard interrupt so DOS doesn't steal our */
    /* keystrokes.                                    */
    rtems_status_code  status;

    status = 
      rtems_interrupt_catch( _IBMPC_keyboard_isr, 9, &old_keyboard_isr );

    if ( status )  {
      int write( int, void *, int );
      void exit( int );

      char msg[] = "error initializing keyboard\n";
      write( 2, msg, sizeof msg - 1 );
      exit( 1 );
    }
  }

  status = rtems_io_register_name(
    "/dev/console",
    major,
    (rtems_device_minor_number) 0
  );
 
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);
 
  atexit( console_cleanup );

  return RTEMS_SUCCESSFUL;
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
 *  Open entry point
 */
 
rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return RTEMS_SUCCESSFUL;
}

/*
 *  Close entry point
 */
 
rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return RTEMS_SUCCESSFUL;
}
 
/*
 * read bytes from the serial port. We only have stdin.
 */
 
rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_libio_rw_args_t *rw_args;
  char *buffer;
  int maximum;
  int count = 0;
 
  rw_args = (rtems_libio_rw_args_t *) arg;
 
  buffer = rw_args->buffer;
  maximum = rw_args->count;
 
  for (count = 0; count < maximum; count++) {
    buffer[ count ] = inbyte();
    if (buffer[ count ] == '\n' || buffer[ count ] == '\r') {
      /* What if this goes past the end of the buffer?  We're hosed. [bhc] */
      buffer[ count++ ]  = '\n';
      buffer[ count ]  = 0;
      break;
    }
  }
 
  rw_args->bytes_moved = count;
  return (count >= 0) ? RTEMS_SUCCESSFUL : RTEMS_UNSATISFIED;
}
 
/*
 * write bytes to the serial port. Stdout and stderr are the same.
 */
 
rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  int count;
  int maximum;
  rtems_libio_rw_args_t *rw_args;
  char *buffer;
 
  rw_args = (rtems_libio_rw_args_t *) arg;
 
  buffer = rw_args->buffer;
  maximum = rw_args->count;
 
  for (count = 0; count < maximum; count++) {
    if ( buffer[ count ] == '\n') {
      outbyte('\r');
    }
    outbyte( buffer[ count ] );
  }

  rw_args->bytes_moved = maximum;
  return 0;
}
 
/*
 *  IO Control entry point
 */
 
rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return RTEMS_SUCCESSFUL;
}

