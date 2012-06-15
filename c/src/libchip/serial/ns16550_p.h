/**
 *  @file
 *  
 */

/*
 *  COPYRIGHT (c) 1998 by Radstone Technology
 *
 *
 *  THIS FILE IS PROVIDED TO YOU, THE USER, "AS IS", WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK
 *  AS TO THE QUALITY AND PERFORMANCE OF ALL CODE IN THIS FILE IS WITH YOU.
 *
 *  You are hereby granted permission to use, copy, modify, and distribute
 *  this file, provided that this notice, plus the above copyright notice
 *  and disclaimer, appears in all copies. Radstone Technology will provide
 *  no support for this code.
 *
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _NS16550_P_H_
#define _NS16550_P_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Define NS16550_STATIC to nothing while debugging so the entry points
 *  will show up in the symbol table.
 */

#define NS16550_STATIC

#define NS16550_RECEIVE_BUFFER   0
#define NS16550_TRANSMIT_BUFFER  0
#define NS16550_INTERRUPT_ENABLE 1
#define NS16550_INTERRUPT_ID     2
#define NS16550_FIFO_CONTROL     2
#define NS16550_LINE_CONTROL     3
#define NS16550_MODEM_CONTROL    4
#define NS16550_LINE_STATUS      5
#define NS16550_MODEM_STATUS     6
#define NS16550_SCRATCH_PAD      7
#define NS16550_FRACTIONAL_DIVIDER 10

/*
 * Define serial port interrupt enable register structure.
 */

#define SP_INT_RX_ENABLE  0x01
#define SP_INT_TX_ENABLE  0x02
#define SP_INT_LS_ENABLE  0x04
#define SP_INT_MS_ENABLE  0x08

#define NS16550_ENABLE_ALL_INTR           (SP_INT_RX_ENABLE | SP_INT_TX_ENABLE)
#define NS16550_DISABLE_ALL_INTR          0x00
#define NS16550_ENABLE_ALL_INTR_EXCEPT_TX (SP_INT_RX_ENABLE)

/*
 * Define serial port interrupt ID register structure.
 */

#define SP_IID_0 0x01
#define SP_IID_1 0x02
#define SP_IID_2 0x04
#define SP_IID_3 0x08

/*
 * Define serial port fifo control register structure.
 */

#define SP_FIFO_ENABLE  0x01
#define SP_FIFO_RXRST 0x02
#define SP_FIFO_TXRST 0x04
#define SP_FIFO_DMA   0x08
#define SP_FIFO_RXLEVEL 0xc0

#define SP_FIFO_SIZE 16

/*
 * Define serial port line control register structure.
 */

#define SP_LINE_SIZE  0x03
#define SP_LINE_STOP  0x04
#define SP_LINE_PAR   0x08
#define SP_LINE_ODD   0x10
#define SP_LINE_STICK 0x20
#define SP_LINE_BREAK 0x40
#define SP_LINE_DLAB  0x80

/*
 * Line status register character size definitions.
 */

#define FIVE_BITS 0x0                   /* five bits per character */
#define SIX_BITS 0x1                    /* six bits per character */
#define SEVEN_BITS 0x2                  /* seven bits per character */
#define EIGHT_BITS 0x3                  /* eight bits per character */

/*
 * Define serial port modem control register structure.
 */

#define SP_MODEM_DTR  0x01
#define SP_MODEM_RTS  0x02
#define SP_MODEM_IRQ  0x08
#define SP_MODEM_LOOP 0x10
#define SP_MODEM_DIV4 0x80

/*
 * Define serial port line status register structure.
 */

#define SP_LSR_RDY    0x01
#define SP_LSR_EOVRUN 0x02
#define SP_LSR_EPAR   0x04
#define SP_LSR_EFRAME 0x08
#define SP_LSR_BREAK  0x10
#define SP_LSR_THOLD  0x20
#define SP_LSR_TX   0x40
#define SP_LSR_EFIFO  0x80

typedef struct {
  uint8_t ucModemCtrl;
  int transmitFifoChars;
} ns16550_context;

/*
 * Driver functions
 */

NS16550_STATIC void ns16550_init(int minor);

NS16550_STATIC int ns16550_open(
  int major,
  int minor,
  void  * arg
);

NS16550_STATIC int ns16550_close(
  int major,
  int minor,
  void  * arg
);

NS16550_STATIC void ns16550_write_polled(
  int   minor,
  char  cChar
);

NS16550_STATIC int ns16550_assert_RTS(
  int minor
);

NS16550_STATIC int ns16550_negate_RTS(
  int minor
);

NS16550_STATIC int ns16550_assert_DTR(
  int minor
);

NS16550_STATIC int ns16550_negate_DTR(
  int minor
);

NS16550_STATIC void ns16550_initialize_interrupts(int minor);

NS16550_STATIC void ns16550_cleanup_interrupts(int minor);

NS16550_STATIC ssize_t ns16550_write_support_int(
  int   minor,
  const char *buf,
  size_t len
);

NS16550_STATIC ssize_t ns16550_write_support_polled(
  int   minor,
  const char *buf,
  size_t len
  );

NS16550_STATIC int ns16550_inbyte_nonblocking_polled(
  int minor
);

NS16550_STATIC void ns16550_enable_interrupts(
  console_tbl *c,
  int         mask
);

NS16550_STATIC int ns16550_set_attributes(
  int                   minor,
  const struct termios *t
);

#ifdef __cplusplus
}
#endif

#endif /* _NS16550_P_H_ */
