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
#include "ringbuf.h"

Ring_buffer_t  Buffer[2];

/*
 *  Interrupt handler for receiver interrupts
 */

rtems_isr C_Receive_ISR(rtems_vector_number vector)
{
  register int    ipend, port;

  ZWRITE0(1, 0x38);     /* reset highest IUS */

  ipend = ZREAD(1, 3);  /* read int pending from A side */

  if      (ipend == 0x04) port = 0;   /* channel B intr pending */
  else if (ipend == 0x20) port = 1;   /* channel A intr pending */
  else return;
    
  Ring_buffer_Add_character(&Buffer[port], ZREADD(port));
  
  if (ZREAD(port, 1) & 0x70) {    /* check error stat */
    ZWRITE0(port, 0x30);          /* reset error */
  }
}

rtems_device_driver console_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor,
  void                      *arg,
  rtems_id                   self,
  rtems_unsigned32          *status
)
{
  int     i;
  
  /*
   * Initialise receiver interrupts on both ports
   */

  for (i = 0; i <= 1; i++) {
    Ring_buffer_Initialize( &Buffer[i] );
    ZWRITE(i, 2, SCC_VECTOR);
    ZWRITE(i, 10, 0);
    ZWRITE(i, 1, 0x10);     /* int on all Rx chars or special condition */
    ZWRITE(i, 9, 8);        /* master interrupt enable */
  }
    
  set_vector(C_Receive_ISR, SCC_VECTOR, 1); /* install ISR for ports A and B */

  mcchip->vector_base = 0;
  mcchip->gen_control = 2;        /* MIEN */
  mcchip->SCC_int_ctl = 0x13;     /* SCC IEN, IPL3 */

  *status = RTEMS_SUCCESSFUL;
}

/*
 *   Non-blocking char input
 */

rtems_boolean char_ready(int port, char *ch)
{
  if ( Ring_buffer_Is_empty( &Buffer[port] ) )
    return FALSE;

  Ring_buffer_Remove_character( &Buffer[port], *ch );
  
  return TRUE;
}

/*
 *   Block on char input
 */

char char_wait(int port)
{
  unsigned char tmp_char;
 
  while ( !char_ready(port, &tmp_char) );
  return tmp_char;
}

/*  
 *   This routine transmits a character out the SCC.  It no longer supports
 *   XON/XOFF flow control.
 */

void char_put(int port, char ch)
{
  while (1) {
    if (ZREAD0(port) & TX_BUFFER_EMPTY) break;
  }
  ZWRITED(port, ch);
}

/*
 *    Map port A (1) to stdin, stdout, and stderr.
 *    Map everything else to port B (0).
 */

int __read(int fd, char *buf, int nbytes)
{
  int i, port;

  if ( fd <= 2 ) port = 1;
  else           port = 0;

  for (i = 0; i < nbytes; i++) {
    *(buf + i) = char_wait(port);
    if ((*(buf + i) == '\n') || (*(buf + i) == '\r')) {
      (*(buf + i++)) = '\n';
      (*(buf + i)) = 0;
      break;
    }
  }
  return (i);
}

/*
 *  Map port A (1) to stdin, stdout, and stderr.
 *  Map everything else to port B (0).
 */

int __write(int fd, char *buf, int nbytes)
{
  int i, port;
 
  if ( fd <= 2 ) port = 1;
  else           port = 0;
 
  for (i = 0; i < nbytes; i++) {
    if (*(buf + i) == '\n') {
      char_put (port, '\r');
    }
    char_put (port, *(buf + i));
  }
  return (nbytes);
}
