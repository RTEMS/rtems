/*
 *  console.c
 *
 *  This driver uses the termios pseudo driver.
 *
 *  Currently only polled mode is supported.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: console.c
 */

#include <stdlib.h>
#include <motorola/mc68681.h>
#include <bsp.h>
#include <rtems/libio.h>
#include <assert.h>

#define COM1                      0
#define COM2                      1
#define NUM_PORTS                 2
#define USE_FOR_CONSOLE           COM1

/*
 *  Define RDB_BREAK_IN if you need to be able to break in to the
 *  program with a ctrl-c during remote target debugging. If so,
 *  UART B will not be accessible from rtems during remote debugging
 *  if interrupt driven console is used. Does not affect UART A, polled 
 *  mode or when the program runs without remote debugging.
 */
#define RDB_BREAK_IN

/* Proto-types for Duart.C */
void console_initialize_interrupts( void );
char console_inbyte_polled( int port );
void console_outbyte_polled(int  port, char ch);
rtems_isr console_isr (rtems_vector_number vector);
volatile void init_mc88681();

/*  PAGE
 *
 *  console_initialize
 *
 *  This routine initializes the console IO driver.
 *
 *  Input parameters:
 *    major - console device major number
 *    minor - console device minor number
 *    arg   - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
 */
 
rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;
  int               console;

  /*
   * initialize the termio interface.
   */
  rtems_termios_initialize();

  /*
   *  Register Device Names
   */
  console = USE_FOR_CONSOLE;
  status = rtems_io_register_name( "/dev/console", major, console );
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  /*
   *  Initialize Hardware
   */

  init_mc88681 ();

#if CONSOLE_USE_INTERRUPTS
  console_initialize_interrupts();
#endif
 
  return RTEMS_SUCCESSFUL;
}

/* PAGE
 *
 *  console_write_support
 *
 *  This routine is Console Termios output entry point.
 *
 *  Input parameters:
 *    minor - console device minor number
 *    buf   - buffer of data to be written
 *    len   - length of data to be written
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    int     number of bytes written
 */

int console_write_support(
  int         minor, 
  const char *buf, 
  int         len)
{
  int                       nwrite = 0;
  int                       port = minor;

  /* 
   * verify port Number 
   */
  assert ( port < NUM_PORTS );

  /*
   * poll each byte in the string out of the port.
   */
  while (nwrite < len) {
#if defined(CONSOLE_USE_INTERRUPTS)
#else
    console_outbyte_polled(port, *buf++);
#endif
   nwrite++;
  }

  /*
   * return the number of bytes written.
   */
  return nwrite;
}


/*  PAGE
 *
 *  DEBUG_puts
 *
 *  This should be safe in the event of an error.  It attempts to insure
 *  that no TX empty interrupts occur while it is doing polled IO.  Then
 *  it restores the state of that external interrupt.
 *
 *  Input parameters:
 *    string  - pointer to debug output string
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 */

void DEBUG_puts(
  char *string
)
{
  char *s;
  rtems_unsigned32    isrlevel;

  rtems_interrupt_disable( isrlevel );
  for ( s = string ; *s ; s++ ) 
    console_outbyte_polled( 0, *s );

  console_outbyte_polled( 0, '\r' );
  console_outbyte_polled( 0, '\n' );
  rtems_interrupt_enable( isrlevel );
}


/* PAGE
 *
 *  console_open
 *
 *  This routine is the console device driver open entry point. 
 *
 *  Input parameters:
 *    major - console device major number
 *    minor - console device minor number
 *    arg   - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
 */
 
rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code sc;
  int               port = minor;
#if defined(CONSOLE_USE_INTERRUPTS)
  rtems_libio_open_close_args_t *args = arg;
#endif

  /*
   * Verify the minor number is valid.
   */
  if (minor < 0)
    return RTEMS_INVALID_NUMBER;

  if ( port > NUM_PORTS )
     return RTEMS_INVALID_NUMBER;

  /*
   *  open the port as a termios console driver.
   */
#if defined(CONSOLE_USE_INTERRUPTS)
   sc = rtems_termios_open (major, minor, arg,
                            NULL, NULL, NULL,
                            console_write_support, 0);
#else
   sc = rtems_termios_open (major, minor, arg, NULL, NULL,
                            console_inbyte_nonblocking,
                            console_write_support,
                            0);
#endif

  return sc;
}
 
 

/* PAGE
 *
 *  console_reserve_resources
 *
 *  This routine reserves resources for each port which may be
 *  used as a console.
 *
 *  Input parameters: 
 *    configuration  - rtems configuration table.
 *
 *  Output parameters: NONE
 *
 *  Return values: NONE
 */

void console_reserve_resources(
  rtems_configuration_table *configuration
)
{
  rtems_termios_reserve_resources( configuration, NUM_PORTS );
}

/* PAGE
 *
 *  console_close
 *
 *  This routine is the console device driver close entry point.
 *
 *  Input parameters:
 *    major - console device major number
 *    minor - console device minor number
 *    arg   - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
 */
 
rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_close (arg);
}
 
/* PAGE
 *
 *  console_read
 *
 *  This routine is the console device driver read entry point.
 *
 *  Input parameters:
 *    major - console device major number
 *    minor - console device minor number
 *    arg   - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
 *
 */
 rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_read (arg);
}
 
/* PAGE
 *
 *  console_write
 *
 *  This routine is the console device driver write entry point.
 *
 *  Input parameters:
 *    major - console device major number
 *    minor - console device minor number
 *    arg   - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
 *
 */
rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write (arg);
}
 
/* PAGE
 *
 *  console_control
 *
 *  This routine is console device driver control entry point
 *
 *  Input parameters:
 *    major - console device major number
 *    minor - console device minor number
 *    arg   - pointer to optional device driver arguments
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    rtems_device_driver status code
 *
 */
rtems_device_driver console_control(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_ioctl (arg);
}


/*
 *  Interrupt driven console IO
 */

#if CONSOLE_USE_INTERRUPTS

/*
 *  Buffers between task and ISRs
 */

#include <ringbuf.h>
extern Ring_buffer_t TX_Buffer[2];
extern Ring_buffer_t RX_Buffer[2];

/*
 *  console_inbyte_interrupts
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
 
char console_inbyte_interrupts( int port )
{
  char ch;

  while ( Ring_buffer_Is_empty( &RX_Buffer[ port ] ) );
 
  Ring_buffer_Remove_character( &RX_Buffer[ port ], ch );
  return ch;
}
 
/*
 *  console_outbyte_interrupts
 *
 *  This routine transmits a character out.
 *
 *  Input parameters:
 *    port - port to transmit character to
 *    ch  - character to be transmitted
 *
 *  Output parameters:  NONE
 *
 *  Return values:      NONE
 */
 
void console_outbyte_interrupts(
  int  port,
  char ch
)
{
  /*
   *  If this is the first character then we need to prime the pump
   */

  if ( Is_TX_active[ port ] == FALSE ) {
    Is_TX_active[ port ] = TRUE;
    console_outbyte_polled( port, ch );
    return;
  }

  while ( Ring_buffer_Is_full( &TX_Buffer[ port ] ) );
 
  Ring_buffer_Add_character( &TX_Buffer[ port ], ch );
}

/*
 *  console_exit
 *
 *  This routine allows the console to exit by masking its associated interrupt
 *  vectors.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */

void console_exit()
{
  volatile unsigned char *_addr;
  int port;

  /*
   *  Although the interrupts for the UART are unmasked, the PIL is set to
   *  disable all external interrupts.  So we might as well do this first.
   */

  /* ??? Mask All UART Interrupts           */

  for (port = MC68681_PORT_A; port <= MC68681_PORT_B; port++) {
    while (!Ring_buffer_Is_empty (&TX_Buffer[port])) {
      Ring_buffer_Remove_character (&TX_Buffer[port],ch);
      console_outbyte_polled (port,ch);
    }
  }

  /*
   *  Now wait for all the data to actually get out ... the send register
   *  should be empty.
   */
   _addr = (unsigned char *) (DUART_ADDR + MC68681_STATUS_REG_A);
   while (!(*_addr & MC68681_TX_EMPTY));

  _addr = (unsigned char *) (DUART_ADDR + MC68681_STATUS_REG_B);
   while (!(*_addr & MC68681_TX_EMPTY));
}
#endif /* CONSOLE_USE_INTERRUPTS */



