/*
 *  duart.c
 *
 *  This code is a modified version of what you will find at the
 *  end of the IDP User's manual.  The original code is copyrighted
 *  by Motorola and Motorola Semiconductor Products as well as
 *  Motorola Software products group.
 *  
 *  Modifications to the original IDP code by Doug McBride, Colorado
 *  Space Grant College.  Modifications include a means of accessing
 *  port B of the duart as well as port A as well as modifications for
 *  buffering and RTEMS support.  Modifications are provided
 *  as is and may not be correct.
 *  
 *  Rob Savoye provided the format for the mc68681 header file
 *  
 *  Joel Sherrill provided inspiration for recoding my original assembly
 *  for this file into C (a good idea)
 *  
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: duart.c
 */

#define MC68681_OFFSET_MULTIPLIER 8
#include <motorola/mc68681.h>
#include <bsp.h>
#include <ringbuf.h>
          
rtems_isr console_isr (rtems_vector_number vector);

Ring_buffer_t TX_Buffer[2];
Ring_buffer_t RX_Buffer[2];

/*  PAGE
 *
 *  init_mc88681
 *
 *  volatile routine to initialize duart
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 *
 *  Return Values:     NONE  
 */

volatile void init_mc88681()
{
  /*
   *  Initialize Ring buffers
   */
  Ring_buffer_Initialize( &RX_Buffer[ 0 ] );
  Ring_buffer_Initialize( &RX_Buffer[ 1 ] );

  Ring_buffer_Initialize( &TX_Buffer[ 0 ] );
  Ring_buffer_Initialize( &TX_Buffer[ 1 ] );
}

/*  PAGE
 *
 *  console_isr
 *
 *  interrupt handler for receive of character from duart on ports A & B 
 *
 *  Input parameters: NONE
 *
 *  Output parameters: NONE
 *
 *  Return Values:     NONE  
 */

rtems_isr console_isr (rtems_vector_number vector)
{

   /*
    *  Fill me in later ...
    */

}


/*  PAGE
 *
 *  console_outbyte_polled
 *
 *  This routine transmits a character out.
 *
 *  Input parameters:
 *    port - port to transmit character to
 *    ch  - character to be transmitted
 *
 *  Output parameters:  NONE
 *
 *  Return values: NONE
 */

void console_outbyte_polled(
  int port,
  char ch
)
{
  unsigned char status;
  unsigned char data;
  unsigned char t = 0;

  if (port == MC68681_PORT_A) { 
    status = MC68681_STATUS_REG_A;
    data = MC68681_TRANSMIT_BUFFER_A;
  } else {
    status = MC68681_STATUS_REG_B;
    data = MC68681_TRANSMIT_BUFFER_B;
  }

  while ( !(MC68681_READ(DUART_ADDR, status) & MC68681_TX_READY) ){
    if (t == 0) {
      Debug_Entry( 0x8000 );
      t++;
    }
  }
  
  Debug_Entry( 0x9000 );
  MC68681_WRITE(DUART_ADDR, data, ch);
}


/*  PAGE
 *
 *  console_inbyte_polled
 *
 *  This routine reads a character from the UART.
 *
 *  Input parameters:
 *    port - port to read character from
 *
 *  Output parameters:  NONE
 *
 *  Return values:
 *    character read from UART
 */

#define MC68681_RECEIVE_ERRORS \
      (MC68681_OVERRUN_ERROR | MC68681_PARITY_ERROR | MC68681_FRAMING_ERROR)

char console_inbyte_polled( int port )
{
  char status;
  char data;
  char cmd;
  unsigned char status_info;

  /*  
   * Set Port A or B unique variables.         
   */
  if (port == MC68681_PORT_A)  {
    status = MC68681_STATUS_REG_A;
    data   = MC68681_RECEIVE_BUFFER_A;
    cmd    = MC68681_COMMAND_REG_A;
  } else {
    status = MC68681_STATUS_REG_B;
    data   = MC68681_RECEIVE_BUFFER_B;
    cmd    = MC68681_COMMAND_REG_B;
  }

  /* Wait for the Ready bit and Clear any errors */
  for ( ; ; ) {
    status_info = MC68681_READ(DUART_ADDR, status);
    if ( status_info & MC68681_RX_READY )
      break;

     if ( status_info & MC68681_RECEIVE_ERRORS )
         MC68681_WRITE( DUART_ADDR, cmd, MC68681_MODE_REG_RESET_ERROR );
  }

  return MC68681_READ(DUART_ADDR, data);
}



















