/*
 *-------------------------------------------------------------------
 *
 *   16550 -- header file for National Semiconducor's 16550 UART
 *
 * This file has been created by John S. Gwynne for the efi68k
 * project.
 * 
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *------------------------------------------------------------------
 *
 *  $Id$
 */

#ifndef _16550_H_
#define _16550_H_

/* base address is the physical location of register 0 */
#define UART_BASE_ADDRESS 0x0400001

/* definitions of register addresses and associate bits */

#define RBR (volatile unsigned char * const)(0*2+UART_BASE_ADDRESS)	
                                /* Receiver Buffer Register (w/DLAB=0)*/
				/*    8-bit data */

#define THR (volatile unsigned char * const)(0*2+UART_BASE_ADDRESS)	
                                /* Transmitter Holding Register (w/DLAB=0) */
				/*    8-bit data */

#define DLL (volatile unsigned char * const)(0*2+UART_BASE_ADDRESS)
                          	/* Divisor Latch (LS) (w/DLAB=1) */
				/*    LSB of Divisor */

#define DLM (volatile unsigned char * const)(1*2+UART_BASE_ADDRESS)
                                /* Divisor Latch (MS) (w/DLAB=1) */
				/*    MSB of Divisor */

#define IER (volatile unsigned char * const)(1*2+UART_BASE_ADDRESS)
                              	/* Interrupt Enable Register (w/DLAB=0) */
#define    ERBFI 0x01		/*    Enable Recv Data Available Interrupt */
#define    ETBEI 0x02		/*    Enable Trans Holding Reg Empty Inter */
#define    ELSI  0x04		/*    Enable Recv Line Status Interrupt */
#define    EDSSI 0x08		/*    Enable Modem Status Interrupt */

#define IIR (volatile unsigned char * const)(2*2+UART_BASE_ADDRESS)
                              	/* Interrupt Ident Register (read only) */
#define    NIP   0x01		/*    No Interrupt Pending */
#define    IID_MASK 0x0e	/*    Interrupt ID mask */
#define    FE_MASK 0xc0		/*    FIFO's Enabled */

#define FCR (volatile unsigned char * const)(2*2+UART_BASE_ADDRESS)
                             	/* FIFO Control Register (write only) */
#define    FIFO_E 0x01		/*    FIFO Enable */
#define    RFR   0x02		/*    RCVR FIFO Reset */
#define    XFR   0x04		/*    XMIT FIFO Reset */
#define    DMAMS 0x08		/*    DMA Mode Select */
#define    RCVRTG_MASK 0xC0	/*    RCVR Triger MSBit/LSBit */

#define LCR (volatile unsigned char * const)(3*2+UART_BASE_ADDRESS)
                               	/* Line Control Register */
#define    WLS_MASK 0x03	/*    Word Legth Select Mask */
#define       WL_5 0x00		/*       5 bits */
#define       WL_6 0x01		/*       6 bits */
#define       WL_7 0x02		/*       7 bits */
#define       WL_8 0x03		/*       8 bits */
#define    NSB   0x04		/*    Number of Stop Bits (set is 2/1.5) */
#define    PEN   0x08		/*    Parity Enable */
#define    EPS   0x10		/*    Even Parity Select */
#define    STP   0x20		/*    Stick Parity */
#define    SETBK 0x40		/*    Set Break */
#define    DLAB  0x80		/*    Divisor Latch Access Bit */

#define MCR (volatile unsigned char * const)(4*2+UART_BASE_ADDRESS)
                              	/* Modem Control Register */
#define    DTR   0x01		/*    Data Terminal Ready */
#define    RTS   0x02		/*    Request to Send */
#define    OUT1  0x04		/*    Out 1 */
#define    OUT2  0x08		/*    Out 2 */
#define    LOOP  0x10		/*    Loop */

#define LSR (volatile unsigned char * const)(5*2+UART_BASE_ADDRESS)
                           	/* Line Status Register */
#define    DR    0x01		/*    Data Ready */
#define    OE    0x02		/*    Overrun error */
#define    PE    0x04		/*    Parity error */
#define    FE    0x08		/*    Framing error */
#define    BI    0x10		/*    Break Interrupt */
#define    THRE  0x20		/*    Transmitter Holding Register */
#define    TEMT  0x40		/*    Transmitter Empty */
#define    RCVFIE 0x80		/*    Recv FIFO Error */

#define MDSR (volatile unsigned char * const)(6*2+UART_BASE_ADDRESS)
                           	/* Modem Status Register */
#define    DCTS  0x01		/*    Delta Clear to Send */
#define    DDSR  0x02		/*    Delta Data Set Ready */
#define    TERI  0x04		/*    Trailing Edge Ring Indicator */
#define    DDCD  0x08		/*    Delta Data Carrier Detect */
#define    CTS   0x10		/*    Clear to Send */
#define    DSR   0x20		/*    Data Set Ready */
#define    RI    0x40		/*    Ring Indicator */
#define    DCD   0x80		/*    Data Carrier Detect */

#define SCR (volatile unsigned char * const)(7*2+UART_BASE_ADDRESS)
                              	/* Scratch Register */
				/*    8-bit register */
#endif
