/**
 *  @file
 *  
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _ISR_ENTRIES_H
#define _ISR_ENTRIES_H 1

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
