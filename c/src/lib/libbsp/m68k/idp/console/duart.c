/*
 *  $Id$
 */

/*#########################################################
#
# This code is a modified version of what you will find at the
# end of the IDP User's manual.  The original code is copyrighted
# by Motorola and Motorola Semiconductor Products as well as
# Motorola Software products group.
#
# Modifications to the original IDP code by Doug McBride, Colorado
# Space Grant College.  Modifications include a means of accessing
# port B of the duart as well as port A as well as modifications for
# buffering and RTEMS support.  Modifications are provided
# as is and may not be correct.
#
# Rob Savoye provided the format for the mc68681 header file
#
# Joel Sherrill provided inspiration for recoding my original assembly
# for this file into C (a good idea)
#
##########################################################*/

#include "mc68230.h"
#include "mc68681.h"
#include "ringbuf.h"
#include "rtems.h"
#include "bsp.h"

rtems_isr C_Receive_ISR(rtems_vector_number vector);
extern Ring_buffer_t  Buffer[];

extern unsigned char inbuf[];
extern unsigned char inbuf_portb[];
extern unsigned tail;
extern unsigned tail_portb;
unsigned char Pit_initialized = 0;

/*#####################################################################
# The volatile routine to initialize the duart -- port a and port b
######################################################################*/
volatile void init_pit()
{
	/* Disable ports A & B while configuring PIT */
	MC68681_WRITE(DUART_IMR, 0x00);		/* disable imr */
	MC68681_WRITE(DUART_CRA, 0x08);		/* disable port a transmitter */
	MC68681_WRITE(DUART_CRA, 0x02);		/* disable port a receiver */
	MC68681_WRITE(DUART_CRB, 0x08);		/* disable port b transmitter */
	MC68681_WRITE(DUART_CRB, 0x02);		/* disable port b receiver */

	/* install ISR for ports A and B */
	set_vector(C_Receive_ISR, (VECT+H3VECT), 1);

	/* initialize pit */
	MC68230_WRITE(PGCR, 0x00);  /* set mode to 0 -- disable all ports */
	MC68230_WRITE(PSRR, 0x18);	/* set up pirq and piack */
	MC68230_WRITE(PBDDR, 0x00);	/* all pins on port b are input */
	MC68230_WRITE(PBCR, 0x82);	/* submode 1x, h3 interrupt enabled */
	MC68230_WRITE(PIVR, VECT);	/* setup pivr */
	MC68230_WRITE(PGCR, 0x20);	/* turn on all ports */

	/* For some reason, the reset of receiver/transmitter only works for
       the first time around -- it garbles the output otherwise (e.g., sp21) */
	if (!Pit_initialized)
	{
	   /* now initialize the duart registers on port b */
	   /* WARNING:OPTIMIZER MAY ONLY EXECUTE THIRD STATEMENT IF NOT VOLATILE */
	   MC68681_WRITE(DUART_CRB, 0x30);		/* reset tx, channel b */
	   MC68681_WRITE(DUART_CRB, 0x20);		/* reset rx, channel b */
	   MC68681_WRITE(DUART_CRB, 0x10);		/* reset mr pointer, channel b */

	   /* now initialize the duart registers on port a */
	   /* WARNING:OPTIMIZER MAY ONLY EXECUTE THIRD STATEMENT IF NOT VOLATILE */
	   MC68681_WRITE(DUART_CRA, 0x30);		/* reset tx, channel a */
	   MC68681_WRITE(DUART_CRA, 0x20);		/* reset rx, channel a */
	   MC68681_WRITE(DUART_CRA, 0x10);		/* reset mr pointer, channel a */
	   Pit_initialized = 1;
	}

	/* init the general registers of the duart */
	MC68681_WRITE(DUART_IVR, 0x0f);		/* init ivr */
	MC68681_WRITE(DUART_IMR, 0x22);		/* init imr */
	MC68681_WRITE(DUART_ACR, 0x00);		/* init acr */
	MC68681_WRITE(DUART_CTUR, 0x00);	/* init ctur */
	MC68681_WRITE(DUART_CTLR, 0x02);	/* init ctlr */
	MC68681_WRITE(DUART_OPCR, 0x00);	/* init opcr */
	MC68681_WRITE(DUART_OPRSET, 0x01);	/* init cts */

	/* init the actual serial port for port a */
	MC68681_WRITE(DUART_CSRA, 0xbb);	/* init csra -- 9600 baud */
	MC68681_WRITE(DUART_MR1A, 0x13);	/* init mr1a */
	MC68681_WRITE(DUART_MR2A, 0x07);	/* init mr2a */
	MC68681_WRITE(DUART_CRA, 0x05);		/* init cra */

	/* init the actual serial port for port b */
	MC68681_WRITE(DUART_CSRB, 0xbb);	/* init csrb -- 9600 baud */
#define EIGHT_BITS_NO_PARITY
#ifdef EIGHT_BITS_NO_PARITY
	MC68681_WRITE(DUART_MR1B, 0x13);	/* init mr1b */
#else /* 7 bits, even parity */
	MC68681_WRITE(DUART_MR1B, 0x02);	/* init mr1b */
#endif
	MC68681_WRITE(DUART_MR2B, 0x07);	/* init mr2b -- one stop bit */
	MC68681_WRITE(DUART_CRB, 0x05);		/* init crb */
}

/*#####################################################################
# interrupt handler for receive of character from duart on ports A & B
#####################################################################*/
rtems_isr C_Receive_ISR(rtems_vector_number vector)
{
	volatile unsigned char *_addr;

	_addr = (unsigned char *) (PIT_ADDR + PITSR);
	*_addr = 0x04;		/* clear pit interrupt */

	/* Let's check port A first for input */
	_addr = (unsigned char *) (DUART_ADDR + DUART_SRA);
	if (*_addr & 0x01)	/* extract rcvrdy on port A */
	{
		/* Read input on port A */
		_addr = (unsigned char *) (DUART_ADDR + DUART_RBA);
		Ring_buffer_Add_character( &Buffer[ 0 ], *_addr );
	}
	else	/* If not on port A, let's check port B */
	{
		_addr = (unsigned char *) (DUART_ADDR + DUART_SRB);
		if (*_addr & 0x01)	/* extract rcvrdy on port B */
		{
			/* Read input on port B */
			_addr = (unsigned char *) (DUART_ADDR + DUART_RBB);
			Ring_buffer_Add_character( &Buffer[ 1 ], *_addr );
		}
		/* if not ready on port A or port B, must be an error */
		/* if error, get out so that fifo is undisturbed */
	}
}

/*#####################################################################
# This is the routine that actually transmits a character one at a time
# This routine transmits on port A of the IDP board
#####################################################################*/
void transmit_char(char ch)
{
	volatile unsigned char *_addr;

	/* Get SRA (extract txrdy) */
	_addr = (unsigned char *) (DUART_ADDR + DUART_SRA);
	while (!(*_addr & 0x04))
	{
	}

	/* transmit character over port A */
	MC68681_WRITE(DUART_TBA, ch);
}

/*#####################################################################
# This is the routine that actually transmits a character one at a time
# This routine transmits on port B of the IDP board
#####################################################################*/
void transmit_char_portb(char ch)
{
	volatile unsigned char *_addr;

	/* Get SRB (extract txrdy) */
	_addr = (unsigned char *) (DUART_ADDR + DUART_SRB);
	while (!(*_addr & 0x04))
	{
	}

	/* transmit character over port B */
	MC68681_WRITE(DUART_TBB, ch);
}
