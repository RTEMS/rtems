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

#define TX3904_IRQ_INT1        MIPS_INTERRUPT_BASE+0
#define TX3904_IRQ_INT2        MIPS_INTERRUPT_BASE+1
#define TX3904_IRQ_INT3        MIPS_INTERRUPT_BASE+2
#define TX3904_IRQ_INT4        MIPS_INTERRUPT_BASE+3
#define TX3904_IRQ_INT5        MIPS_INTERRUPT_BASE+4
#define TX3904_IRQ_INT6        MIPS_INTERRUPT_BASE+5
#define TX3904_IRQ_INT7        MIPS_INTERRUPT_BASE+6
#define TX3904_IRQ_DMAC3       MIPS_INTERRUPT_BASE+7
#define TX3904_IRQ_DMAC2       MIPS_INTERRUPT_BASE+8
#define TX3904_IRQ_DMAC1       MIPS_INTERRUPT_BASE+9
#define TX3904_IRQ_DMAC0       MIPS_INTERRUPT_BASE+10
#define TX3904_IRQ_SIO0        MIPS_INTERRUPT_BASE+11
#define TX3904_IRQ_SIO1        MIPS_INTERRUPT_BASE+12
#define TX3904_IRQ_TMR0        MIPS_INTERRUPT_BASE+13
#define TX3904_IRQ_TMR1        MIPS_INTERRUPT_BASE+14
#define TX3904_IRQ_TMR2        MIPS_INTERRUPT_BASE+15
#define TX3904_IRQ_INT0        MIPS_INTERRUPT_BASE+16
#define TX3904_IRQ_SOFTWARE_1  MIPS_INTERRUPT_BASE+17
#define TX3904_IRQ_SOFTWARE_2  MIPS_INTERRUPT_BASE+18
#define TX3904_MAXIMUM_VECTORS MIPS_INTERRUPT_BASE+19

#endif
