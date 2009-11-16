/*
 *  $Id$
 */

#ifndef _ISR_ENTRIES_H
#define _ISR_ENTRIES_H 1

extern void mips_install_isr_entries( void );

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
