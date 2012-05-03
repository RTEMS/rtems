/*
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _MG5UART_H_
#define _MG5UART_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This is the ASCII for "MG5U" which should be unique enough to
 *  distinguish this type of serial device from others.
 */

#define SERIAL_MG5UART 0x474D5535

#define MG5UART_UART0  0
#define MG5UART_UART1  1

/*
 *  These are just used in the interface between this driver and
 *  the read/write register routines when accessing the first
 *  control port.  They are indices of registers from the bases.
 */

/* shared registers from peripheral base (i.e. from ulCtrlPort1) */
/*
#define MG5UART_COMMAND_REGISTER          0
#define MG5UART_STATUS_REGISTER           1
#define MG5UART_INTERRUPT_CAUSE_REGISTER  2
#define MG5UART_INTERRUPT_MASK_REGISTER   3
*/

#define MG5UART_COMMAND_REGISTER          0
#define MG5UART_STATUS_REGISTER           0x04
#define MG5UART_INTERRUPT_CAUSE_REGISTER  0x08
#define MG5UART_INTERRUPT_MASK_REGISTER   0x0C

/* port specific registers from uart base (i.e. from ulCtrlPort2) */
#define MG5UART_RX_BUFFER  0
#define MG5UART_TX_BUFFER  4
#define MG5UART_BAUD_RATE  8

/*
 *  Interrupt mask values
 */

#define MG5UART_ENABLE_ALL_EXCEPT_TX MONGOOSEV_UART_ALL_RX_STATUS_BITS

/* all rx ints on, but only tx ready.  no need to also int on tx empty */
#define MG5UART_ENABLE_ALL           (MONGOOSEV_UART_ALL_STATUS_BITS & ~MONGOOSEV_UART_TX_EMPTY)

#define MG5UART_DISABLE_ALL          0x0000

/*
 *  Assume vectors are sequential.
 */

#define MG5UART_IRQ_RX_FRAME_ERROR   0
#define MG5UART_IRQ_RX_OVERRUN_ERROR 1
#define MG5UART_IRQ_TX_EMPTY         2
#define MG5UART_IRQ_TX_READY         3
#define MG5UART_IRQ_RX_READY         4
/*
 * Driver function table
 */

extern console_fns mg5uart_fns;
extern console_fns mg5uart_fns_polled;

/*
 * Default register access routines
 */

uint32_t   mg5uart_get_register(    /* registers are on 32-bit boundaries */
  uint32_t    ulCtrlPort,           /*   and accessed as word             */
  uint32_t    ucRegNum
);

void  mg5uart_set_register(
  uint32_t    ulCtrlPort,
  uint32_t    ucRegNum,
  uint32_t    ucData
);

#ifdef __cplusplus
}
#endif

#endif /* _MG5UART_H_ */
