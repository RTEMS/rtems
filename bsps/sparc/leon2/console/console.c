/**
 * @file
 * @ingroup RTEMSBSPsSPARCLEON2
 * @brief TTY driver driver for the serial ports on the LEON console
 *
 *  This file contains the TTY driver for the serial ports on the LEON.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>
#include <assert.h>

#include <rtems/console.h>
#include <rtems/libio.h>
#include <bsp.h>

/*
 *  Interrupt driven console IO
 */

#if (CONSOLE_USE_INTERRUPTS)

/*
 *  Buffers between task and ISRs
 */

#include <rtems/ringbuf.h>

Ring_buffer_t  TX_Buffer[ 2 ];
bool           Is_TX_active[ 2 ];

void *console_termios_data[ 2 ];

/*
 *  console_isr_a
 *
 *  This routine is the console interrupt handler for Channel 1.
 *
 *  Input parameters:
 *    vector - vector number
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */

rtems_isr console_isr_a(
  rtems_vector_number vector
)
{
  char ch;
  int UStat;

  if ( (UStat = LEON_REG.UART_Status_1) & LEON_REG_UART_STATUS_DR ) {
    if (UStat & LEON_REG_UART_STATUS_ERR) {
      LEON_REG.UART_Status_1 = LEON_REG_UART_STATUS_CLR;
    }
    ch = LEON_REG.UART_Channel_1;

    rtems_termios_enqueue_raw_characters( console_termios_data[ 0 ], &ch, 1 );
  }

  if ( LEON_REG.UART_Status_1 & LEON_REG_UART_STATUS_THE ) {
    if ( !Ring_buffer_Is_empty( &TX_Buffer[ 0 ] ) ) {
      Ring_buffer_Remove_character( &TX_Buffer[ 0 ], ch );
      LEON_REG.UART_Channel_1 = (uint32_t) ch;
    } else
     Is_TX_active[ 0 ] = false;
  }

  LEON_Clear_interrupt( LEON_INTERRUPT_UART_1_RX_TX );
}

/*
 *  console_isr_b
 *
 *  This routine is the console interrupt handler for Channel 2.
 *
 *  Input parameters:
 *    vector - vector number
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */

rtems_isr console_isr_b(
  rtems_vector_number vector
)
{
  char ch;
  int UStat;

  if ( (UStat = LEON_REG.UART_Status_2) & LEON_REG_UART_STATUS_DR ) {
    if (UStat & LEON_REG_UART_STATUS_ERR) {
      LEON_REG.UART_Status_2 = LEON_REG_UART_STATUS_CLR;
    }
    ch = LEON_REG.UART_Channel_2;
    rtems_termios_enqueue_raw_characters( console_termios_data[ 1 ], &ch, 1 );

  }

  if ( LEON_REG.UART_Status_2 & LEON_REG_UART_STATUS_THE ) {
    if ( !Ring_buffer_Is_empty( &TX_Buffer[ 1 ] ) ) {
      Ring_buffer_Remove_character( &TX_Buffer[ 1 ], ch );
      LEON_REG.UART_Channel_2 = (uint32_t) ch;
    } else
     Is_TX_active[ 1 ] = false;
  }

  LEON_Clear_interrupt( LEON_INTERRUPT_UART_2_RX_TX );
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
  uint32_t port;
  uint32_t ch;

  /*
   *  Although the interrupts for the UART are unmasked, the PIL is set to
   *  disable all external interrupts.  So we might as well do this first.
   */

  LEON_Mask_interrupt( LEON_INTERRUPT_UART_1_RX_TX );
  LEON_Mask_interrupt( LEON_INTERRUPT_UART_2_RX_TX );

  for ( port=0 ; port <= 1 ; port++ ) {
    while ( !Ring_buffer_Is_empty( &TX_Buffer[ port ] ) ) {
      Ring_buffer_Remove_character( &TX_Buffer[ port ], ch );
      console_outbyte_polled( port, ch );
    }
  }

  /*
   *  Now wait for all the data to actually get out ... the send register
   *  should be empty.
   */

  while ( (LEON_REG.UART_Status_1 & LEON_REG_UART_STATUS_THE) !=
          LEON_REG_UART_STATUS_THE );

  while ( (LEON_REG.UART_Status_2 & LEON_REG_UART_STATUS_THE) !=
          LEON_REG_UART_STATUS_THE );

  LEON_REG.UART_Control_1 = 0;
  LEON_REG.UART_Control_2 = 0;
  LEON_REG.UART_Status_1 = 0;
  LEON_REG.UART_Status_2 = 0;


}

#define CONSOLE_UART_1_TRAP  LEON_TRAP_TYPE( LEON_INTERRUPT_UART_1_RX_TX )
#define CONSOLE_UART_2_TRAP  LEON_TRAP_TYPE( LEON_INTERRUPT_UART_2_RX_TX )

/*
 *  console_initialize_interrupts
 *
 *  This routine initializes the console's receive and transmit
 *  ring buffers and loads the appropriate vectors to handle the interrupts.
 *
 *  Input parameters:  NONE
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */

#ifdef RDB_BREAK_IN
  extern uint32_t trap_table[];
#endif

void console_initialize_interrupts( void )
{
  Ring_buffer_Initialize( &TX_Buffer[ 0 ] );
  Ring_buffer_Initialize( &TX_Buffer[ 1 ] );

  Is_TX_active[ 0 ] = false;
  Is_TX_active[ 1 ] = false;

  atexit( console_exit );

  LEON_REG.UART_Control_1 |= LEON_REG_UART_CTRL_RI | LEON_REG_UART_CTRL_TI;
  LEON_REG.UART_Control_2 |= LEON_REG_UART_CTRL_RI | LEON_REG_UART_CTRL_TI;

  set_vector( console_isr_a, CONSOLE_UART_1_TRAP, 1 );
#ifdef RDB_BREAK_IN
  if (trap_table[0x150/4] == 0x91d02000)
#endif
  set_vector( console_isr_b, CONSOLE_UART_2_TRAP, 1 );
}

/*
 *  console_outbyte_interrupt
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

void console_outbyte_interrupt(
  int   port,
  char  ch
)
{
  /*
   *  If this is the first character then we need to prime the pump
   */

  if ( Is_TX_active[ port ] == false ) {
    Is_TX_active[ port ] = true;
    console_outbyte_polled( port, ch );
    return;
  }

  while ( Ring_buffer_Is_full( &TX_Buffer[ port ] ) );

  Ring_buffer_Add_character( &TX_Buffer[ port ], ch );
}

#endif /* CONSOLE_USE_INTERRUPTS */

/*
 *  Console Termios Support Entry Points
 *
 */

static ssize_t console_write_support (int minor, const char *buf, size_t len)
{
  int nwrite = 0;

  while (nwrite < len) {
#if (CONSOLE_USE_INTERRUPTS)
    console_outbyte_interrupt( minor, *buf++ );
#else
    console_outbyte_polled( minor, *buf++ );
#endif
    nwrite++;
  }
  return nwrite;
}

/*
 *  Console Device Driver Entry Points
 *
 */

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg
)
{
  rtems_status_code status;

  rtems_termios_initialize();

  /*
   *  Register Device Names
   */

  status = rtems_io_register_name( "/dev/console", major, 0 );
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  status = rtems_io_register_name( "/dev/console_b", major, 1 );
  if (status != RTEMS_SUCCESSFUL)
    rtems_fatal_error_occurred(status);

  /*
   *  Initialize Hardware
   */

  LEON_REG.UART_Control_1 |= LEON_REG_UART_CTRL_RE | LEON_REG_UART_CTRL_TE;
  LEON_REG.UART_Control_2 |= LEON_REG_UART_CTRL_RE | LEON_REG_UART_CTRL_TE |
      LEON_REG_UART_CTRL_RI;  /* rx irq default enable for remote debugger */
  LEON_REG.UART_Status_1 = 0;
  LEON_REG.UART_Status_2 = 0;
#if (CONSOLE_USE_INTERRUPTS)
  console_initialize_interrupts();
#endif

  return RTEMS_SUCCESSFUL;
}

rtems_device_driver console_open(
  rtems_device_major_number major,
  rtems_device_minor_number minor,
  void                    * arg
)
{
  rtems_status_code sc;
#if (CONSOLE_USE_INTERRUPTS)
  rtems_libio_open_close_args_t *args = arg;
  static const rtems_termios_callbacks intrCallbacks = {
    NULL,                        /* firstOpen */
    NULL,                        /* lastClose */
    NULL,                        /* pollRead */
    console_write_support,       /* write */
    NULL,                        /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    0                            /* outputUsesInterrupts */
  };
#else
  static const rtems_termios_callbacks pollCallbacks = {
    NULL,                        /* firstOpen */
    NULL,                        /* lastClose */
    console_inbyte_nonblocking,  /* pollRead */
    console_write_support,       /* write */
    NULL,                        /* setAttributes */
    NULL,                        /* stopRemoteTx */
    NULL,                        /* startRemoteTx */
    0                            /* outputUsesInterrupts */
  };
#endif

  assert( minor <= 1 );
  if ( minor > 2 )
    return RTEMS_INVALID_NUMBER;

#if (CONSOLE_USE_INTERRUPTS)
  sc = rtems_termios_open (major, minor, arg, &intrCallbacks);

  console_termios_data[ minor ] = args->iop->data1;
#else
  sc = rtems_termios_open (major, minor, arg, &pollCallbacks);
#endif
  (void) sc; /* avoid set but not used warning */

  return RTEMS_SUCCESSFUL;
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
