/*
 *  MIPS Tx3904 specific information
 *
 *  NOTE: This is far from complete.  --joel (13 Dec 2000)
 *
 *  $Id$
 */

#ifndef __TX3904_h
#define __TX3904_h

/*
 *  Timer Base Addresses and Offsets
 */

#define TX3904_TIMER0_BASE 0xFFFFF000
#define TX3904_TIMER1_BASE 0xFFFFF100
#define TX3904_TIMER2_BASE 0xFFFFF200

#define TX3904_TIMER_TCR  0x00
#define TX3904_TIMER_TISR 0x04
#define TX3904_TIMER_CPRA 0x08
#define TX3904_TIMER_CPRB 0x0C
#define TX3904_TIMER_ITMR 0x10
#define TX3904_TIMER_CCDR 0x20
#define TX3904_TIMER_PGMR 0x30
#define TX3904_TIMER_WTMR 0x40
#define TX3904_TIMER_TRR  0xF0

#define TX3904_TIMER_READ( _base, _register ) \
  *((volatile unsigned32 *)((_base) + (_register)))

#define TX3904_TIMER_WRITE( _base, _register, _value ) \
  *((volatile unsigned32 *)((_base) + (_register))) = (_value)

/*
 *  Interrupt Vector Numbers
 *
 *  NOTE: Numbers 0-15 directly map to levels on the IRC.
 *        Number 16 is "1xxxx" per p. 164 of the TX3904 manual.
 */

#define TX3904_IRQ_INT1        0
#define TX3904_IRQ_INT2        1
#define TX3904_IRQ_INT3        2
#define TX3904_IRQ_INT4        3
#define TX3904_IRQ_INT5        4
#define TX3904_IRQ_INT6        5
#define TX3904_IRQ_INT7        6
#define TX3904_IRQ_DMAC3       7
#define TX3904_IRQ_DMAC2       8
#define TX3904_IRQ_DMAC1       9
#define TX3904_IRQ_DMAC0      10
#define TX3904_IRQ_SIO0       11
#define TX3904_IRQ_SIO1       12
#define TX3904_IRQ_TMR0       13
#define TX3904_IRQ_TMR1       14
#define TX3904_IRQ_TMR2       15
#define TX3904_IRQ_INT0       16
#define TX3904_IRQ_SOFTWARE_1 17
#define TX3904_IRQ_SOFTWARE_2 18

#endif
