/**
 *  @file
 *  
 *  MIPS Tx3904 specific information
 *
 *  NOTE: This is far from complete.  --joel (13 Dec 2000)
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
  *((volatile uint32_t*)((_base) + (_register)))

#define TX3904_TIMER_WRITE( _base, _register, _value ) \
  *((volatile uint32_t*)((_base) + (_register))) = (_value)

#endif
