/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
