/*
 * bat.h
 *
 *	    This file contains declaration of C function to
 *          Instantiate 60x/7xx ppc Block Address Translation (BAT) registers.
 *	    More detailed information can be found on motorola
 *	    site and more precisely in the following book :
 *
 *		MPC750
 *		Risc Microporcessor User's Manual
 *		Motorola REF : MPC750UM/AD 8/97
 *
 * Copyright (C) 1999  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _LIBCPU_BAT_H
#define _LIBCPU_BAT_H

#include <libcpu/mmu.h>
#include <libcpu/pgtable.h>

#define IO_PAGE	(_PAGE_NO_CACHE | _PAGE_GUARDED | _PAGE_RW)

#ifndef ASM
/* Take no risks -- the essential parts of this routine run with
 * interrupts disabled!
 *
 * The routine does basic parameter checks:
 *   - Index must be 0..3 (0..7 on 7455, 7457).
 *     If an index > 3 is requested the 745x is
 *     programmed to enable the higher BATs.
 *   - Size must be a power of two and <= 1<<28
 *     (<=1<<31 on 7455, 7457. Also, on these processors
 *     the special value 0xffffffff is allowed which stands
 *     for 1<<32).
 *     If a size > 1<<28 is requested, the 745x is
 *     programmed to enable the larger block sizes.
 *   - Bat ranges must not overlap.
 *   - Physical & virtual addresses must be aligned
 *     to the size.
 *
 * RETURNS: zero on success, nonzero on failure.
 */
extern int setdbat(int bat_index, unsigned long virt, unsigned long phys,
		    unsigned int size, int flags);

/* Same as setdbat but sets IBAT */
extern int setibat(int bat_index, unsigned long virt, unsigned long phys,
		    unsigned int size, int flags);

/* read DBAT # 'idx' into *pu / *pl. NULL pointers may be passed.
 * If pu and pl are NULL, the bat contents are dumped to the console (printk).
 *
 * RETURNS: upper BAT contents or (-1) if index is invalid
 */
extern int getdbat(int bat_index, unsigned long *pu, unsigned long *pl);

/* Same as getdbat but reads IBAT */
extern int getibat(int bat_index, unsigned long *pu, unsigned long *pl);

/* Do not use the asm-routines; they are obsolete; use setdbat() instead */
extern void asm_setdbat0(unsigned int uperPart, unsigned int lowerPart);
extern void asm_setdbat1(unsigned int uperPart, unsigned int lowerPart);
extern void asm_setdbat2(unsigned int uperPart, unsigned int lowerPart);
extern void asm_setdbat3(unsigned int uperPart, unsigned int lowerPart);
#else

/* Initialize all bats (upper and lower) to zero. This routine should *only*
 * be called during early BSP initialization when no C-ABI is available
 * yet.
 * This routine clobbers r3 and r4.
 * NOTE: on 7450 CPUs all 8 dbat/ibat units are cleared. On 601 CPUs only
 *       4 ibats.
 */
	.globl CPU_clear_bats_early
	.type  CPU_clear_bats_early,@function

#endif

#endif /* _LIBCPU_BAT_H */
