/*
 *  This file contains the TTY driver for the serial ports on the SCORE603e.
 *
 *  This driver uses the termios pseudo driver.
 *
 *  Currently only polled mode is supported.
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>

#include "consolebsp.h"

#if (1)
/*
 * The Port Used for the Console interface is based upon which 
 * debugger is being used.  The SDS debugger uses a binary 
 * interface on port 0 as part of the debugger.  Thus port 0 can
 * not be used as the console port for the SDS debugger.
 */

#if (SCORE603E_USE_SDS)
#define USE_FOR_CONSOLE_DEF  1

#elif (SCORE603E_USE_OPEN_FIRMWARE)
#define USE_FOR_CONSOLE_DEF  0

#elif (SCORE603E_USE_NONE)
#define USE_FOR_CONSOLE_DEF  0

#elif (SCORE603E_USE_DINK)
#define USE_FOR_CONSOLE_DEF  0

#else
#error "SCORE603E CONSOLE.C -- what ROM monitor are you using"
#endif

#endif

#if (0)
extern int USE_FOR_CONSOLE;
#endif

int USE_FOR_CONSOLE = USE_FOR_CONSOLE_DEF;

/*
 *
 *  Console Device Driver Entry Points
 */
 
/* PAGE
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
  int   console;
  volatile rtems_unsigned8 *csr;

  console = USE_FOR_CONSOLE;

  csr = Ports_85C30[ console ].ctrl;

  /* should disable interrupts here */

  for ( s = string ; *s ; s++ ) 
    outbyte_polled_85c30( csr, *s );

  outbyte_polled_85c30( csr, '\r' );
  outbyte_polled_85c30( csr, '\n' );

  /* should enable interrupts here */
}

/* PAGE
 *
 *  console_inbyte_nonblocking 
 *
 *  Console Termios polling input entry point.
 */

int console_inbyte_nonblocking( 
  int minor 
)
{
  int                       port = minor;

  /* 
   * verify port Number 
   */
  assert ( port < NUM_Z85C30_PORTS );
 
  /*
   * return a character from the 85c30 port.
   */
  return inbyte_nonblocking_85c30( &Ports_85C30[ port ] );
}
 
rtems_device_driver console_close(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_close (arg);
}
 
rtems_device_driver console_read(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_read (arg);
}
 
rtems_device_driver console_write(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  return rtems_termios_write (arg);
}
 
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

rtems_isr console_isr(
  rtems_vector_number vector
)
{
  int  i;
  
  for (i=0; i < NUM_Z85C30_PORTS; i++){
      ISR_85c30_Async( &Ports_85C30[i] );

#if (0) /* XXX - TO TEST LOOP BACKS comment this out. */
    if ( Ports_85C30[i].Chip->vector == vector ) {
      ISR_85c30_Async( &Ports_85C30[i] );
    }
#endif
  }  
}

void console_exit()
{
  int i;
  volatile Ring_buffer_t *buffer;
  rtems_unsigned32 ch;

  for ( i=0 ; i < NUM_Z85C30_PORTS ; i++ ) {
    
    buffer = &( Ports_85C30[i].Protocol->TX_Buffer);

    while ( !Ring_buffer_Is_empty( buffer ) ) {
      Ring_buffer_Remove_character( buffer, ch );
      outbyte_polled_85c30( Ports_85C30[i].ctrl, ch ); 
    }
  }
}

void console_initialize_interrupts( void )
{
  volatile Ring_buffer_t     *buffer;
  Console_Protocol  *protocol;
  int               i;
  
  for ( i=0 ; i < NUM_Z85C30_PORTS ; i++ ) {
    protocol = Ports_85C30[i].Protocol;

    /*
     * Initialize the ring buffer and set to not transmitting.
     */
    buffer = &protocol->TX_Buffer;
    Ring_buffer_Initialize( buffer );
    protocol->Is_TX_active = FALSE;
  }

  /* 
   * Connect each vector to the interupt service routine.
   */
  for (i=0; i < NUM_Z85C30_CHIPS; i++)
    set_vector( console_isr, Chips_85C30[i].vector, 1 );
  

  atexit( console_exit );
   
}
void console_outbyte_interrupts(
  const Port_85C30_info *Port,
  char ch
);

/* XXXXXX */
#endif


/* PAGE
 *
 *  console_initialize
 *
 *  Routine called to initialize the console device driver.
 */
rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code          status;
  rtems_device_minor_number  console;
  int                        port, chip, p0,p1;

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


/*
 * INITIALIZE_COM_PORTS is defined in the linker script.  If it is
 * true all serial chips on the board are to be reset at startup
 * otherwise the reset is assumed to occur elsewhere (ie. in the
 * debugger...)
 */
#if ( INITIALIZE_COM_PORTS )

  /*
   * Force to perform a hardware reset w/o
   * Master interrupt enable via register 9
   */
  
  for (port=0; port<NUM_Z85C30_PORTS; port++){
    p0 = port;
    port++;
    p1 = port;
    Reset_85c30_chip( Ports_85C30[p0].ctrl, Ports_85C30[p1].ctrl );
  }
#else
  /* TEMP - To see if this makes a diff with the new ports. 
   *        Never reset chip 1 when using the chip as a monitor
   */
  for (port=2; port<NUM_Z85C30_PORTS; port++){
    p0 = port;
    port++;
    p1 = port;
    Reset_85c30_chip( Ports_85C30[p0].ctrl, Ports_85C30[p1].ctrl );
  }
#endif

  /* 
   * Initialize each port.
   * Note:  the ports are numbered such that 0,1 are on the first chip
   *        2,3 are on the second ....
   */

  for (port=0; port<NUM_Z85C30_PORTS; port++) {
    chip = port >> 1;
    initialize_85c30_port( &Ports_85C30[port] );
  }

#if CONSOLE_USE_INTERRUPTS
  console_initialize_interrupts();
#endif

  return RTEMS_SUCCESSFUL;
}

/* PAGE
 *
 *  console_write_support
 *
 *  Console Termios output entry point.
 *
 */
int console_write_support(
  int   minor, 
  const char *buf, 
  int   len)
{
  int nwrite = 0;
  volatile rtems_unsigned8 *csr;
  int                       port = minor;

  /* 
   * verify port Number 
   */
  assert ( port < NUM_Z85C30_PORTS );

  /*
   * Set the csr based upon the port number.
   */
  csr = Ports_85C30[ port ].ctrl;

  /*
   * poll each byte in the string out of the port.
   */
  while (nwrite < len) {
#if (CONSOLE_USE_INTERRUPTS)
    console_outbyte_interrupts( &Ports_85C30[ port ], *buf++ );
#else
    outbyte_polled_85c30( csr, *buf++ );
#endif
   nwrite++;
  }

  /*
   * return the number of bytes written.
   */
  return nwrite;
}

/* PAGE
 *
 *  console_open
 *
 *  open a port as a termios console.
 *
 */
rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code sc;
  int               port = minor;
#if (CONSOLE_USE_INTERRUPTS)
  rtems_libio_open_close_args_t *args = arg;
  static const rtems_termios_callbacks intrCallbacks = {
    NULL,                       /* firstOpen */
    NULL,                       /* lastClose */
    NULL,                       /* pollRead */
    console_write_support,      /* write */
    NULL,                       /* setAttributes */
    NULL,                       /* stopRemoteTx */
    NULL,                       /* startRemoteTx */
    1                           /* outputUsesInterrupts */
  };
#else
  static const rtems_termios_callbacks pollCallbacks = {
    NULL,                       /* firstOpen */
    NULL,                       /* lastClose */
    console_inbyte_nonblocking, /* pollRead */
    console_write_support,      /* write */
    NULL,                       /* setAttributes */
    NULL,                       /* stopRemoteTx */
    NULL,                       /* startRemoteTx */
    0                           /* outputUsesInterrupts */
  };
#endif


  /*
   * Verify the minor number is valid.
   */
  if (minor < 0)
    return RTEMS_INVALID_NUMBER;

  if ( port > NUM_Z85C30_PORTS )
     return RTEMS_INVALID_NUMBER;

  /*
   *  open the port as a termios console driver.
   */

#if (CONSOLE_USE_INTERRUPTS)
   sc = rtems_termios_open( major, minor, arg, &intrCallbacks );

   Ports_85C30[ minor ].Protocol->console_termios_data = args->iop->data1;
#else
   sc = rtems_termios_open( major, minor, arg, &pollCallbacks );
#endif

  return sc;
}


#if (CONSOLE_USE_INTERRUPTS)

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
  const Port_85C30_info *Port,
  char ch
)
{
  Console_Protocol   *protocol;
  rtems_unsigned32    isrlevel;

  protocol = Port->Protocol;
  
  /*
   *  If this is the first character then we need to prime the pump
   */

  if ( protocol->Is_TX_active == FALSE ) {

    rtems_interrupt_disable( isrlevel );
    protocol->Is_TX_active = TRUE;
    outbyte_polled_85c30( Port->ctrl, ch );
    rtems_interrupt_enable( isrlevel );

    return;
  }

  while ( Ring_buffer_Is_full( &protocol->TX_Buffer ) );
 
  Ring_buffer_Add_character( &protocol->TX_Buffer, ch );
}

#endif











