/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *  
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

#ifndef _ISR_ENTRIES_H
#define _ISR_ENTRIES_H 1

#include <rtems/score/cpuimpl.h>

extern void mips_install_isr_entries( void );
extern void mips_vector_isr_handlers( CPU_Interrupt_frame *frame );

#if __mips == 1
extern void exc_utlb_code(void);
extern void exc_dbg_code(void);
extern void exc_norm_code(void);
#elif __mips == 32
extern void exc_tlb_code(void);
extern void exc_xtlb_code(void);
extern void exc_cache_code(void);
extern void exc_norm_code(void);
#elif __mips == 3
extern void exc_tlb_code(void);
extern void exc_xtlb_code(void);
extern void exc_cache_code(void);
extern void exc_norm_code(void);
#endif

#endif
