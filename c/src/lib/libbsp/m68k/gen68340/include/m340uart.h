/*
 *  Header file for console driver
 *  defines for accessing M68340/68349 UART registers
 *
 *  Author:
 *  Geoffroy Montel
 *  France Telecom - CNET/DSM/TAM/CAT
 *  4, rue du Clos Courtel
 *  35512 CESSON-SEVIGNE
 *  FRANCE
 *
 *  e-mail: g_montel@yahoo.com
 *
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __m340uart_H__
#define __m340uart_H__

/* UART initialisation */
#define UART_CHANNEL_A			0
#define UART_CHANNEL_B			1
#define UART_NUMBER_OF_CHANNELS		2
#define UART_CONSOLE_NAME		"/dev/console"
#define UART_RAW_IO_NAME		"/dev/tty1"
#define UART_FIFO_FULL			0
#define UART_CRR			1
#define UART_INTERRUPTS			0
#define UART_POLLING			1
#define UART_TERMIOS_CONSOLE		0
#define UART_TERMIOS_RAW		1
#define UART_TERMIOS_MIN_DEFAULT	1
#define UART_TERMIOS_TIME_DEFAULT	0

void Init_UART_Table(void);

typedef struct {
		uint8_t        		enable;
		uint16_t        	rx_buffer_size;	/* NOT IMPLEMENTED */
		uint16_t        	tx_buffer_size;	/* NOT IMPLEMENTED */
	       } uart_termios_config;

typedef struct { /* for one channel */
		uint8_t        		enable;		/* use this channel */
		char			name[64];	/* use UART_CONSOLE_NAME for console purpose */
		uint8_t        		parity_mode;	/* parity mode, see MR1 section for defines */
		uint8_t        		bits_per_char;	/* bits per character, see MR1 section for defines  */
		float			rx_baudrate;	/* Rx baudrate */
		float			tx_baudrate;	/* Tx baudrate */
		uint8_t        		rx_mode;	/* FIFO Full (UART_FIFO_FULL) or ChannelReceiverReady (UART_CRR) */
		uint8_t        		mode;		/* use interrupts (UART_INTERRUPTS) or polling (UART_POLLING) */
		uart_termios_config	termios;
	       } uart_channel_config;

extern uart_channel_config		m340_uart_config[UART_NUMBER_OF_CHANNELS];

typedef  struct {
		 int	set;	/* number of the m340 baud speed set */
		 int	rcs;	/* RCS for the needed baud set */
		 int    tcs;	/* TCS for the needed baud set */
		} t_baud_speed;

typedef	 struct {
		 t_baud_speed	baud_speed_table[2];
		 short		nb;
		} t_baud_speed_table;

extern t_baud_speed_table
Find_Right_m340_UART_Config(float ChannelA_ReceiverBaudRate, float ChannelA_TransmitterBaudRate, uint8_t         enableA,
			    float ChannelB_ReceiverBaudRate, float ChannelB_TransmitterBaudRate, uint8_t         enableB);

extern rtems_isr InterruptHandler (rtems_vector_number v);

extern int dbugRead (int minor);
extern ssize_t dbugWrite (int minor, const char *buf, size_t len);

extern float m340_Baud_Rates_Table[16][2];

/*  SR */
#define m340_Rx_RDY		1
#define m340_FFULL		(1<<1)
#define m340_Tx_RDY		(1<<2)
#define m340_TxEMP		(1<<3)
#define m340_OE			(1<<4)
#define m340_PE			(1<<5)
#define m340_FE			(1<<6)
#define m340_RB			(1<<7)

/*  IER */
#define m340_TxRDYA		1
#define m340_RxRDYA		(1<<1)
#define m340_TxRxRDYA		0x3
#define m340_TxRDYB		(1<<4)
#define m340_RxRDYB		(1<<5)
#define m340_TxRxRDYB		0x30

/*  CR */
#define m340_Reset_Error_Status	0x40
#define m340_Reset_Receiver	0x20
#define m340_Reset_Transmitter	0x30
#define m340_Transmitter_Enable	(1<<2)
#define m340_Receiver_Enable	1
#define m340_Transmitter_Disable (2<<2)
#define m340_Receiver_Disable	2

/*  ACR */
#define m340_BRG_Set1		0
#define m340_BRG_Set2		(1<<7)

/*  OPCR */
#define m340_OPCR_Gal		0x0
#define m340_OPCR_Aux		0xFF

/*  ISR */
#define m340_COS		(1<<7)
#define m340_DBB		(1<<6)
#define m340_XTAL_RDY		(1<<3)
#define m340_DBA		(1<<2)

/*  MR1 */
#define m340_RxRTS		(1<<7)
#define m340_R_F		(1<<6)		/* character or block mode */
#define m340_ERR		(1<<5)
#define m340_RxRTX		(1<<7)
#define m340_Even_Parity	0
#define m340_Odd_Parity		(1<<2)
#define m340_Low_Parity		(2<<2)
#define m340_High_Parity	(3<<2)
#define m340_No_Parity		(4<<2)
#define m340_Data_Character	(6<<2)
#define m340_Address_Character	(7<<2)
#define m340_5bpc		0x0
#define m340_6bpc		0x1
#define m340_7bpc		0x2
#define m340_8bpc		0x3

/*  MR2 */
#define m340_normal		(0<<6)
#define m340_automatic_echo	(1<<6)
#define m340_local_loopback	(2<<6)
#define m340_remote_loopback	(3<<6)
#define m340_TxRTS		(1<<5)
#define m340_TxCTS		(1<<4)

/* Baud rates for Transmitter/Receiver */
#define SCLK	1		/* put your own SCLK value here */

#endif
