/*
 *  $Id$
 */

#include "serial.h"
#include "rtems.h"


typedef unsigned char uchar ;           /* Abbreviations */
typedef unsigned short ushort ;
typedef unsigned long ulong ;

#if 0
#define BAUDRate	9600		/* Fixed Uart baud rate */
#endif

#define SEND_WAIT	0x0100		/* Waiting to send character */

#define TDR(i)


/********************************************************************
 ***  16552 specific DUART definitions.
 *******************************************************************/


typedef struct uart_16552 DUART ;

#ifndef notdef
struct uart_16552
{
	short	u_short[8*2] ;
} ;
#define u_reg(n)	u_short[2*(n)]

#else

struct uart_16552
{
	int	u_int[8] ;
} ;

#define u_reg(n)	u_int[(n)]
#endif

#define u_tdr		u_reg(0)	/* Transmit Data Register (write) */
#define u_rdr		u_reg(0)	/* Receive Data Register (read) */
#define u_dlr0		u_reg(0)	/* Divisor Latch Register (lsb) */

#define u_ier		u_reg(1)	/* Interrupt Enable Register */
#define u_dlr1		u_reg(1)	/* Divisor Latch Register (msb) */

#define u_iir		u_reg(2)	/* Interrupt ID Register (read) */
#define u_fcr		u_reg(2)	/* FIFO Control Register (write) */
#define u_afr		u_reg(2)	/* Alternate Funct Reg (read/write) */

#define u_lcr		u_reg(3)	/* Line Control Register */

#define u_mcr		u_reg(4)	/* Modem Control Register */

#define u_lsr		u_reg(5)	/* Line Status Register */

#define u_msr		u_reg(6)	/* Modem Status Register */

#define u_spr		u_reg(7)	/* Scratch Pad Register */

#define uart1	((volatile DUART *)0x90000380)
#define uart2	((volatile DUART *)0x90000300)

#define NUM_UARTS 2
static volatile DUART * duart[NUM_UARTS] = { uart1, uart2 };


extern void	display_msg(void);
/*extern int	sprintf();*/


#define board_rev_reg  ((volatile short *)0x90000080)

static unsigned int shift_val = 0;

/***********************************************************************
 ***  16552 DUART initialization routine.
 ***********************************************************************/

int
console_duartinit(unsigned int uart_num, unsigned int BAUDRate)
{
	register uchar	tmp;
	unsigned int board_rev = *board_rev_reg & 0xff;

        switch( BAUDRate )
        {
           case 1200:
           case 2400:
           case 9600:
           case 19200:
           case 38400:
           case 57600:
              break;
           default:
              /* unknown baud rate */
              return FALSE;
        }

        /* the board rev register should never be 0xff.
           if it equals 0xff, assume that we're on old hardware
           that needs all values shifted by 8. */
        if ( board_rev == 0xff )
           shift_val = 8;
        else
           shift_val = 0;

	if ( uart_num >= NUM_UARTS )
	   return -1;

	duart[uart_num]->u_lcr = 0x80<<shift_val ;	/* Set DLAB bit to 1 */

	duart[uart_num]->u_dlr0 = ((115200 / BAUDRate) >> 0)<<shift_val ; /* Set baud		   */
	duart[uart_num]->u_dlr1 = ((115200 / BAUDRate) >> 8)<<shift_val ; /*          rate 	   */

	duart[uart_num]->u_lcr = 0x03<<shift_val ;	/* 8 bits, no parity, 1 stop */

	duart[uart_num]->u_mcr = 0x0b<<shift_val ;	/* Assert RTS, DTR & OUT2	  */

	duart[uart_num]->u_fcr = 0x00<<shift_val ;	/* Clear 16552 FIFOs */
	/* Is the following write of 0x01 needed? */
	/* Let's try it without... */
	duart[uart_num]->u_fcr = 0xc7<<shift_val ;	/* Enable 16552 FIFOs */

	duart[uart_num]->u_ier = 0x07<<shift_val ;	/* Enable transmit/receive ints	*/

	tmp = duart[uart_num]->u_lsr ;		/* Re-arm interrupts		*/
	tmp = duart[uart_num]->u_rdr ;
	tmp = duart[uart_num]->u_msr ;

	return(0);
}

	/*------------ end of duartinit function ----------------*/


    /***********************************************************************
     ***  Transmit character to host.
     ***********************************************************************/

int console_sps_putc(unsigned int uart_num, int ch)
{
	register unsigned short	stat;

	if ( uart_num >= NUM_UARTS )
	   return -1;

	/*
	 *  Pause until there is room in the UART transmit
	 *  buffer.
	 */

	do {
		stat = duart[uart_num]->u_lsr>>shift_val;
	} while (!(stat & 0x40));

	/*
	 *  Transmit data. (Junk)
	 */

	TDR(ch)

	duart[uart_num]->u_tdr = ch<<shift_val ;

	return ch;

}


    /***********************************************************************
     ***  Read character from host.
     ***********************************************************************/

int console_sps_getc(unsigned int uart_num)
{
	register unsigned short	stat;
	register int	ch;

	if ( uart_num >= NUM_UARTS )
	   return -1;

	stat = duart[uart_num]->u_lsr>>shift_val;
	while (!(stat & 0x01))
        {
                rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
		stat = duart[uart_num]->u_lsr>>shift_val;
        }

	ch = duart[uart_num]->u_rdr>>shift_val;

	return ch;
}

    /***********************************************************************
     ***  check character from host.
     ***********************************************************************/

int console_sps_kbhit(unsigned int uart_num)
{
        register unsigned short stat;

        if ( uart_num >= NUM_UARTS )
           return -1;

        stat = duart[uart_num]->u_lsr>>shift_val;
        return ((stat & 0x01));
}




