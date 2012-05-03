
/*
 * This software is Copyright (C) 1998 by T.sqware - all rights limited
 * It is provided in to the public domain "as is", can be freely modified
 * as far as this copyight notice is kept unchanged, but does not imply
 * an endorsement by T.sqware of the product in which it is included.
 *
 * Copyright (c) Canon Research France SA.]
 * Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _BSPUART_H
#define _BSPUART_H

void BSP_uart_init(int uart, int baud, int hwFlow);
void BSP_uart_set_baud(int aurt, int baud);
void BSP_uart_intr_ctrl(int uart, int cmd);
void BSP_uart_throttle(int uart);
void BSP_uart_unthrottle(int uart);
int  BSP_uart_polled_status(int uart);
void BSP_uart_polled_write(int uart, int val);
int  BSP_uart_polled_read(int uart);
void BSP_uart_termios_set(int uart, void *ttyp);
int  BSP_uart_termios_write_com1(int minor, const char *buf, int len);
int  BSP_uart_termios_write_com2(int minor, const char *buf, int len);
void BSP_uart_termios_isr_com1();
void BSP_uart_termios_isr_com2();
void BSP_uart_dbgisr_com1(void);
void BSP_uart_dbgisr_com2(void);
extern unsigned BSP_poll_char_via_serial(void);
extern void BSP_output_char_via_serial(int val);
extern int BSPConsolePort;
extern int BSPBaseBaud;
/*
 * Command values for BSP_uart_intr_ctrl(),
 * values are strange in order to catch errors
 * with assert
 */
#define BSP_UART_INTR_CTRL_DISABLE  (0)
#define BSP_UART_INTR_CTRL_GDB      (0xaa) /* RX only */
#define BSP_UART_INTR_CTRL_ENABLE   (0xbb) /* Normal operations */
#define BSP_UART_INTR_CTRL_TERMIOS  (0xcc) /* RX & line status */

/* Return values for uart_polled_status() */
#define BSP_UART_STATUS_ERROR    (-1) /* No character */
#define BSP_UART_STATUS_NOCHAR   (0)  /* No character */
#define BSP_UART_STATUS_CHAR     (1)  /* Character present */
#define BSP_UART_STATUS_BREAK    (2)  /* Break point is detected */

/* PC UART definitions */
#define BSP_UART_COM1            (0)
#define BSP_UART_COM2            (1)

/*
 * Base IO for UART
 */

#define COM1_BASE_IO	0x3F8
#define COM2_BASE_IO	0x2F8

/*
 * Offsets from base
 */

/* DLAB 0 */
#define RBR  RSRBR   /* Rx Buffer Register (read) */
#define THR  RSTHR   /* Tx Buffer Register (write) */
#define IER  RSIER   /* Interrupt Enable Register */

/* DLAB X */
#define IIR  RSIIR   /* Interrupt Ident Register (read) */
#define FCR  RSFCR   /* FIFO Control Register (write) */
#define LCR  RSLCR   /* Line Control Register */
#define LSR  RSLSR   /* Line Status Register */

/* DLAB 1 */
#define DLL  RSDLL   /* Divisor Latch, LSB */
#define DLM  RSDLH   /* Divisor Latch, MSB */

/* Uart control */
#define CNT  RSCNT   /* General Control register */

/*
 * define bit for CNT
 */
#define UART_ENABLE 	1
#define PAD_ENABLE	2

/*
 * Interrupt source definition via IIR
 */
#define NO_MORE_INTR				1
#define TRANSMITTER_HODING_REGISTER_EMPTY	2
#define RECEIVER_DATA_AVAIL			4
#define RECEIVER_ERROR  			6
#define CHARACTER_TIMEOUT_INDICATION		12

/*
 * Bits definition of IER
 */
#define	RECEIVE_ENABLE		0x1
#define	TRANSMIT_ENABLE		0x2
#define RECEIVER_LINE_ST_ENABLE	0x4
#define INTERRUPT_DISABLE	0x0

/*
 * Bits definition of the Line Status Register (LSR)
 */
#define DR	0x01	/* Data Ready */
#define OE	0x02	/* Overrun Error */
#define PE	0x04	/* Parity Error */
#define FE	0x08	/* Framing Error */
#define BI	0x10	/* Break Interrupt */
#define THRE	0x20	/* Transmitter Holding Register Empty */
#define TEMT	0x40	/* Transmitter Empty */
#define ERFIFO	0x80	/* Error receive Fifo */

/*
 * Bits definition of the Line Control Register (LCR)
 */
#define CHR_5_BITS 0
#define CHR_6_BITS 1
#define CHR_7_BITS 2
#define CHR_8_BITS 3

#define WL	0x03	/* Word length mask */
#define STB	0x04	/* 1 Stop Bit, otherwise 2 Stop Bits */
#define PEN	0x08	/* Parity Enabled */
#define EPS	0x10	/* Even Parity Select, otherwise Odd */
#define SP	0x20	/* Stick Parity */
#define BCB	0x40	/* Break Control Bit */
#define DLAB	0x80	/* Enable Divisor Latch Access */

/*
 * Bits definition of the FIFO Control Register : WD16C552 or NS16550
 */

#define FIFO_CTRL   0x01    /* Set to 1 permit access to other bits */
#define FIFO_EN	    0x01    /* Enable the FIFO */
#define XMIT_RESET  0x04    /* Transmit FIFO Reset */
#define RCV_RESET   0x02    /* Receive FIFO Reset */
#define FCR3	    0x08    /* do not understand manual! */

#define RECEIVE_FIFO_TRIGGER1	0x0  /* trigger recieve interrupt after 1 byte  */
#define RECEIVE_FIFO_TRIGGER4	0x40 /* trigger recieve interrupt after 4 byte  */
#define RECEIVE_FIFO_TRIGGER8	0x80 /* trigger recieve interrupt after 8 byte  */
#define RECEIVE_FIFO_TRIGGER12	0xc0 /* trigger recieve interrupt after 14 byte */
#define TRIG_LEVEL  		0xc0 /* Mask for the trigger level 		*/

#endif /* _BSPUART_H */
