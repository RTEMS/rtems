/*
 *  FFUART for PXA250 CPU by Yang Xi<hiyangxi@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __FFUART_H__
#define __FFUART_H__

typedef struct {
  /*
   *Receive buffer(DLAB=0).Transmit buffer(DLAB=0).
   *Divisor Latch Low(DLAB=1)
   */
  volatile unsigned int rbr;
  /*Interrupt enable(DLAB=0). Divisor Latch High(DLAB=1)*/
  volatile unsigned int ier;
  /*Interrupt identification.FIFO control*/
  volatile unsigned int iir;
  /*Line Control*/
  volatile unsigned int lcr;
  /*Modem control*/
  volatile unsigned int mcr;
  /*Line Status*/
  volatile unsigned int lsr;
  /*Modem status*/
  volatile unsigned int msr;
  /*Scratch Pad*/
  volatile unsigned int spr;
  /*Infrared Selection*/
  volatile unsigned int isr;
} ffuart_reg_t;


#define EIGHT_BITS_NOPARITY_1STOPBIT  0x3
#define DLAB 0x80


/*Divisor = frequency_uart/(16 * BaudRate*)*/
#define FREQUENCY_UART (14745600)

#define SEND_EMPTY 0x20
#define FULL_RECEIVE 0x01

#endif

