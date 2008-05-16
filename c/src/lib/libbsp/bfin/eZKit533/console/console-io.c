/*  console-io.c
 * 
 *  This file contains the hardware specific portions of the TTY driver
 *  for the serial ports for ezkit533.
 *
 *  Copyright (c) 2006 by Atos Automacao Industrial Ltda.
 *             written by Alain Schaefer <alain.schaefer@easc.ch>
 *                    and Antonio Giovanini <antonio@atos.com.br>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
 

#include <bsp.h>
#include <cplb.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

/*
 *  console_initialize_hardware
 *
 *  This routine initializes the console hardware.
 *
 */

/* 
 * console_initialize_hardware does not compile with optimizations.
 * Makefile.am has been changed to compile the console driver with -O0. 
 */
void console_initialize_hardware(void)
{
  unsigned int divisor;
  
  /* Set DLAB to access divisor registers */
  *((uint16_t*)UART_LCR) = DLAB;

  /* Set divisor registers
   * BAUDRATE = SCLK/(16xDIVISOR)
   */
  divisor = SCLK/(BAUDRATE*16);                /* 1300000/(16x57600) */
  *((uint16_t*)UART_DLL) = (divisor & 0x00ff);
  *((uint16_t*)UART_DLH) = ((divisor >> 8) & 0x00ff);

  /* Set Control Register - 8 bits, no parity, 1 stop bit*/
  *((uint16_t*)UART_LCR) = WORD_8BITS;

  /* Enable Serial Clocks */
  *((uint16_t*)UART_GCTL) = UCEN;

  return;
}

/*
 *  console_outbyte_polled
 *
 *  This routine transmits a character using polling.
 */

void console_outbyte_polled(
  int  port,
  char ch
)
{
  while (!((*((uint16_t*)UART_LSR))&THRE)){} /* Wait for sending last byte */
  *((uint16_t*)UART_THR) = ch;
}

/*
 *  console_inbyte_nonblocking
 *
 *  This routine polls for a character.
 */

int console_inbyte_nonblocking(
  int port
)
{
  char c;
  
  if ( *((uint16_t*)UART_LSR) & DR){
    c = *((uint16_t*)UART_RBR);
  }else{
    return -1;
  }

  return c;
}

#if (CONSOLE_USE_INTERRUPTS)

#include <rtems/ringbuf.h>

Ring_buffer_t  TX_Buffer[ 1 ];
boolean        Is_TX_active[ 1 ];

extern void *console_termios_data[ 1 ];
/*
 *  console_isr
 *
 *  This routine is the console interrupt handler for Channel A.
 *
 *  Input parameters:
 *    vector - vector number
 *
 *  Output parameters: NONE
 *
 *  Return values:     NONE
 */
rtems_isr console_isr(
  rtems_vector_number vector
)
{
  char ch;
  uint16_t UStat;

  UStat = *((uint16_t*)UART_IIR);

  if ( ((UStat&0x06) == 0x04) && ( (*((uint16_t*)UART_LSR)) & DR)) {
    ch = *((uint16_t*)UART_RBR);
    rtems_termios_enqueue_raw_characters( console_termios_data[ 0 ], &ch, 1 );
  }

  if ( ((UStat&0x06) == 0x02) && ((*((uint16_t*)UART_LSR))&THRE) ) {
    if ( !Ring_buffer_Is_empty( &TX_Buffer[ 0 ] ) ) {
      Ring_buffer_Remove_character( &TX_Buffer[ 0 ], ch );
      *((uint16_t*)UART_THR) = ch;
    } else {
      Is_TX_active[ 0 ] = FALSE;
    }
  }
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
  uint32_t         port;
  uint16_t         ch;

  /*
   *  Although the interrupts for the UART are unmasked, the PIL is set to
   *  disable all external interrupts.  So we might as well do this first.
   */

  /* Disable UART interruptions */
  *((uint16_t*)UART_IER) = 0x0000; 
  *((uint16_t*)SIC_IMASK) &= 0xFFFF3FBF; 

  while ( !Ring_buffer_Is_empty( &TX_Buffer[ port ] ) ) {
    Ring_buffer_Remove_character( &TX_Buffer[ port ], ch );
    console_outbyte_polled( port, ch );
  }

  /*
   *  Now wait for all the data to actually get out ... the send register
   *  should be empty.
   */

  while ( !((*((uint16_t*)UART_LSR))&THRE) );
}

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

/*#ifdef RDB_BREAK_IN
  extern uint32_t   trap_table[];
#endif*/

void console_initialize_interrupts( void )
{
  Ring_buffer_Initialize( &TX_Buffer[ 0 ] );

  Is_TX_active[ 0 ] = FALSE;

  atexit( console_exit );

  /* Setting priority - IVG10*/
  *((uint32_t*)SIC_IAR0)  &= 0xF0FFFFFF;
  *((uint32_t*)SIC_IAR0)  |= 0x03000000;
  *((uint32_t*)SIC_IAR1)  &= 0x00FFFFFF;
  *((uint32_t*)SIC_IAR1)  |= 0x33000000;
  
  /* System Interrupt Mask */
  *((uint32_t*)SIC_IMASK) |= 0x0000C040;
  
  set_vector( console_isr, UART_VECTOR, 1 );
  
  /* Enable UART Interruption
   * ELSI - RX error interrupt
   * ETBEI - TX buffer empty
   * ERBFI - RX buffer full 
   */
  *((uint16_t*)UART_IER) = ELSI | ETBEI | ERBFI;
}

void console_outbyte_interrupt(
  int   port,
  char  ch
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

#endif

#include <rtems/bspIo.h>

void bfsim_BSP_output_char(char c) { console_outbyte_polled( 0, c ); }

BSP_output_char_function_type           BSP_output_char = bfsim_BSP_output_char;
BSP_polling_getchar_function_type       BSP_poll_char = NULL;
