/*
 * ioaddr.h - 16C450 serial port memory-mapped I/O address definitions
 *
 *                   THIS SOFTWARE IS NOT COPYRIGHTED
 *
 *  The following software is offered for use in the public domain.
 *  There is no warranty with regard to this software or its performance
 *  and the user must accept the software "AS IS" with all faults.
 *
 *  THE CONTRIBUTORS DISCLAIM ANY WARRANTIES, EXPRESS OR IMPLIED, WITH
 *  REGARD TO THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef	_IOADDR_H
#define	_IOADDR_H

/*
 * The following addresses are implementation-specific.
 * Note that big-endian memory addressing is assumed.
 */
#define	ISA_IO_BASE	0xb8000000
#define	BYTE_IO(ioaddr)	((ioaddr<<3)+7)
#define	WORD_IO(ioaddr)	((ioaddr<<3)+6)

#define	ISA_IRQ9_RESET	0xb8043000
#define	ISA_IRQ5_RESET	0xb8042000
#define	ISA_IRQ4_RESET	0xb8041000
#define	ISA_IRQ3_RESET	0xb8040000
#define	ISA_IRQ_STATUS	0xb8040000

/* ISA IRQ Status Register fields */
#define	ISA_IRQ9_STATUS	0x08
#define	ISA_IRQ5_STATUS	0x04
#define	ISA_IRQ4_STATUS	0x02
#define	ISA_IRQ3_STATUS	0x01

/*
 *  Serial Port 1 (COM1) I/O Addresses.  These definitions
 *  follows the standard IBM AT I/O address assignments.
 *  Note that IRQ4 is normally assigned to serial port 1.
 */
#define	DIV_LO_COM1	BYTE_IO(0x3f8)	/* Div latch lo (line ctl bit 7 = 1) */
#define	DIV_HI_COM1	BYTE_IO(0x3f9)	/* Div latch hi (line ctl bit 7 = 1) */
#define	DATA_REG_COM1	BYTE_IO(0x3f8)	/* TX Buf (write)/RX Buf (read) */
#define	INT_ENA_COM1	BYTE_IO(0x3f9)	/* Interrupt Enable Register */
#define	INT_ID_COM1	BYTE_IO(0x3fa)	/* Interrupt ID Register */
#define	LINE_CTL_COM1	BYTE_IO(0x3fb)	/* Line Control Register */
#define	MODEM_CTL_COM1	BYTE_IO(0x3fc)	/* Modem Control Register */
#define	LINE_STS_COM1	BYTE_IO(0x3fd)	/* Line Status Register */
#define	MODEM_STS_COM1	BYTE_IO(0x3fe)	/* Modem Status Register */

/*
 *  Serial Port 2 (COM1) I/O Addresses.  These definitions
 *  follows the standard IBM AT I/O address assignments.
 *  Note that IRQ3 is normally assigned to serial port 2.
 */
#define	DIV_LO_COM2	BYTE_IO(0x2f8)	/* Div latch lo (line ctl bit 7 = 1) */
#define	DIV_HI_COM2	BYTE_IO(0x2f9)	/* Div latch hi (line ctl bit 7 = 1) */
#define	DATA_REG_COM2	BYTE_IO(0x2f8)	/* TX Buf (write)/RX Buf (read) */
#define	INT_ENA_COM2	BYTE_IO(0x2f9)	/* Interrupt Enable Register */
#define	INT_ID_COM2	BYTE_IO(0x2fa)	/* Interrupt ID Register */
#define	LINE_CTL_COM2	BYTE_IO(0x2fb)	/* Line Control Register */
#define	MODEM_CTL_COM2	BYTE_IO(0x2fc)	/* Modem Control Register */
#define	LINE_STS_COM2	BYTE_IO(0x2fd)	/* Line Status Register */
#define	MODEM_STS_COM2	BYTE_IO(0x2fe)	/* Modem Status Register */

/* Interrupt Enable Register fields */
#define	IENA_MODEM	0x08
#define	IENA_LINE	0x04
#define	IENA_TX		0x02
#define	IENA_RX		0x01

/* Interrupt Identification Register fields */
#define	INT_ID_MASK	0x06
#define	INT_PENDING	0x01

/* Line Control Register fields */
#define	DIV_LATCH_EN	0x80
#define	SET_BREAK	0x40
#define	STICK_PARITY	0x20
#define	EVEN_PARITY	0x10
#define	PARITY_ENA	0x08
#define	STOP_BITS	0x04
#define	WORD_LEN_MASK	0x03

/* Line Status Register fields */
#define	TX_SHR_EMPTY	0x40
#define	TX_BUF_EMPTY	0x20
#define	BRK_RCVD	0x10
#define	FRAMING_ERR	0x08
#define	PARITY_ERR	0x04
#define	OVRUN_ERR	0x02
#define	RX_CHAR_AVA	0x01

/* Modem Control Register fields */
#define	LOOPBACK	0x10
#define	OUT2		0x08
#define	OUT1		0x04
#define	RTS		0x02
#define	DTR		0x01

/* Modem Status Register fields */
#define	RLSD		0x80
#define	RI		0x40
#define	DSR		0x20
#define	CTS		0x10
#define	DELTA_RLSD	0x08
#define	TRAIL_EDGE_RI	0x04
#define	DELTA_DSR	0x02
#define	DELTA_CTS	0x01

#endif	/* _IOADDR_H */
